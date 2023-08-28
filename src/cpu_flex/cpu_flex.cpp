#include "cpu_flex.h"

#include "stages/stage_if.h"
#include "stages/stage_id.h"
#include "stages/stage_ex.h"
#include "stages/stage_mem.h"
#include "stages/stage_wb.h"

#include "../utils.h"

#include<cassert>
#include <memory>

using namespace std;

namespace mips_sim {

    CpuFlex::CpuFlex(std::shared_ptr<Memory> _memory,
                 std::shared_ptr<ControlUnit> _control_unit,
                 int branch_type,
                 int branch_stage,
                 bool has_forwarding_unit,
                 bool has_hazard_detection_unit)
    : CpuPipelined(_memory)
    {   
        fu = std::shared_ptr<ForwardingUnit>(new ForwardingUnit(control_unit, has_forwarding_unit));
        hdu = std::shared_ptr<HazardDetectionUnit>(new HazardDetectionUnit(control_unit, has_hazard_detection_unit));

        hardware_manager = std::shared_ptr<HardwareManager>(new HardwareManager(BRANCH_NON_TAKEN, STAGE_ID));

        StageIF* if_stage = new StageIF(memory, hardware_manager, control_unit, sr_bank);
        StageID* id_stage = new StageID(control_unit, hardware_manager,
                                        {SIG_MEM2REG, SIG_REGBANK, SIG_REGWRITE,
                                        SIG_MEMREAD, SIG_MEMWRITE,
                                        SIG_BRANCH, SIG_PCSRC, SIG_ALUSRC, SIG_ALUOP, SIG_REGDST},
                                        hdu, fu, gpr_bank, fpr_bank, sr_bank);
        StageEX* ex_stage = new StageEX(status["mult-delay"], status["div-delay"], control_unit, hardware_manager,
                                        {SIG_MEM2REG, SIG_REGBANK, SIG_REGWRITE,
                                        SIG_MEMREAD, SIG_MEMWRITE,
                                        SIG_BRANCH, SIG_PCSRC},
                                        fu, sr_bank, gpr_bank);
        StageMEM* mem_stage = new StageMEM(memory, control_unit, hardware_manager, {SIG_MEM2REG, SIG_REGBANK, SIG_REGWRITE}, sr_bank);
        StageWB* wb_stage = new StageWB(control_unit, hardware_manager, gpr_bank, fpr_bank);

        ex_stage->set_syscall(std::bind(&CpuFlex::syscall, this, std::placeholders::_1));

        if_stage->set_sigmask(control_unit->get_signal_bitmask(signals_ID, 10));
        ex_stage->set_sigmask(control_unit->get_signal_bitmask(signals_ID, 7));
        mem_stage->set_sigmask(control_unit->get_signal_bitmask(signals_ID, 3));

        fu->set_seg_reg_ex_mem(mem_stage->get_seg_reg());
        fu->set_seg_reg_mem_wb(wb_stage->get_seg_reg());

        hdu->set_seg_reg_id_ex(ex_stage->get_seg_reg());
        hdu->set_seg_reg_ex_mem(mem_stage->get_seg_reg());

        add_cpu_stage(if_stage);
        add_cpu_stage(id_stage);
        add_cpu_stage(ex_stage);
        add_cpu_stage(mem_stage);
        add_cpu_stage(wb_stage); 
    }

  
    CpuFlex::~CpuFlex() {}

    std::vector<CpuStage*> CpuFlex::get_cpu_stages() {
        return cpu_stages;
    }

    void CpuFlex::add_cpu_stage(CpuStage* stage) {
        cpu_stages.push_back(stage);
    }

    bool CpuFlex::next_cycle( std::ostream &out )
    {
        Cpu::next_cycle( std::cout );

        for (auto stage: cpu_stages) {
            stage->rising_flank();
        }

        for (auto stage: cpu_stages) {
            stage->work_h();
        }

        for (auto stage: cpu_stages) {
            stage->work_l();
        }

        // update diagram
        for (size_t stage_id = 0; stage_id < STAGE_COUNT; ++stage_id)
        {
            size_t iindex = get_current_instruction(stage_id);
            if (iindex >= MAX_DIAGRAM_SIZE || cycle >= MAX_DIAGRAM_SIZE)
            {
                // TODO: Allow for disabling diagram? Cyclic buffer?
                //throw Exception::e(OVERFLOW_EXCEPTION, "Overflow in multicycle diagram");
            }
            diagram[iindex][cycle] = static_cast<uint32_t>(stage_id+1);
        }
        print_diagram(std::cout);
        // update segmentation registers
        cpu_stages.at(STAGE_ID)->set_seg_reg(cpu_stages.at(STAGE_IF)->get_next_seg_reg());
        cpu_stages.at(STAGE_EX)->set_seg_reg(cpu_stages.at(STAGE_ID)->get_next_seg_reg());
        cpu_stages.at(STAGE_MEM)->set_seg_reg(cpu_stages.at(STAGE_EX)->get_next_seg_reg());
        cpu_stages.at(STAGE_WB)->set_seg_reg(cpu_stages.at(STAGE_MEM)->get_next_seg_reg());

        if (int stages_to_flush = hardware_manager->get_signal(SIGNAL_FLUSH)() > 0)
        {
            for (int i = 1; i <= stages_to_flush; ++i)
                cpu_stages.at(i)->set_seg_reg({});
        }

        return ready;
    }

    void CpuFlex::print_diagram( std::ostream &out ) const
    {   
        std::vector<uint32_t> loaded_inst = get_loaded_instructions();
        for (uint32_t inst: loaded_inst) {
            std::cout << inst << std::endl;
        }
        for (size_t i = 1; i < loaded_inst.size(); ++i)
        {   
            uint32_t ipc = loaded_inst[i];
            uint32_t icode = ipc?memory->mem_read_32(ipc):0;
            uint32_t iindex = (ipc - MEM_TEXT_START)/4 + 1;
            out << right << iindex << " " << left << Utils::decode_instruction(icode) << "  ";
            int runstate = 0;
            for (size_t j = 1; j<=cycle && runstate < 2; j++)
                if (diagram[i][j] > 0)
                {
                    runstate = 1;
                    if (diagram[i][j] == diagram[i][j-1])
                        out << "--";
                    else
                        out << stage_names[diagram[i][j]-1];
                }
                else
                {
                    if (runstate)
                        runstate = 2;
                    out << "";
                }
            out << endl;
        }
  }

    void CpuFlex::reset(bool reset_data_memory, bool reset_text_memory) {
        Cpu::reset(reset_data_memory, reset_text_memory);

        //reset seg_regs
        for (auto stage: cpu_stages){
            stage->set_seg_reg({});
        }

        //reset diagram
        for (size_t i=0; i<MAX_DIAGRAM_SIZE; ++i)
            memset(diagram[i], 0, MAX_DIAGRAM_SIZE * sizeof(uint32_t));
        
        //reset hm
        hardware_manager->reset();
        
        for (auto stage: cpu_stages) {
            stage->reset();
        }
    }

    size_t CpuFlex::get_current_instruction(size_t stage) const
    {
        assert(stage <= STAGE_WB);

        uint32_t instruction_code;
        if (stage == STAGE_IF)
        {
            instruction_code = static_cast<StageIF*>(cpu_stages.at(stage))->get_loaded_instruction_index();
        }
        else
        {
            instruction_code = cpu_stages.at(stage)->get_seg_reg()->data[SR_IID];
        }

        return instruction_code;
    }

    std::vector<uint32_t> CpuFlex::get_loaded_instructions() const {
        return static_cast<StageIF*>(cpu_stages.at(STAGE_IF))->get_loaded_instructions();
    }

    bool CpuFlex::run_to_cycle( uint32_t target_cycle, std::ostream& out) {
        std::ostream nullostream(nullptr);

        /* reset CPU and memory */
        reset( true, true );

        if (target_cycle > 0)
        {
            while(cycle < (target_cycle-1) && ready)
            {
            if (!next_cycle(nullostream))
                return false;
            }
            next_cycle(out);
        }
        return true;
    }

} //namespace