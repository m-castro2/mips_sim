#include "cpu_flex.h"

#include "stages/stage_if.h"
#include "stages/stage_id.h"
#include "stages/stage_ex.h"
#include "stages/stage_mem.h"
#include "stages/stage_wb.h"

#include "../utils.h"

#include<cassert>
#include <memory>

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
                                        hdu, fu, sr_bank);
        StageEX* ex_stage = new StageEX(status["mult-delay"], status["div-delay"], control_unit, hardware_manager,
                                        {SIG_MEM2REG, SIG_REGBANK, SIG_REGWRITE,
                                        SIG_MEMREAD, SIG_MEMWRITE},
                                        fu, sr_bank);
        StageMEM* mem_stage = new StageMEM(memory, control_unit, hardware_manager, {SIG_MEM2REG, SIG_REGBANK, SIG_REGWRITE}, sr_bank);
        StageWB* wb_stage = new StageWB(control_unit, hardware_manager);

        fu->set_seg_reg_ex_mem(ex_stage->get_seg_reg());
        fu->set_seg_reg_mem_wb(mem_stage->get_seg_reg());

        hdu->set_seg_reg_id_ex(id_stage->get_seg_reg());
        hdu->set_seg_reg_ex_mem(ex_stage->get_seg_reg());

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
        Cpu::next_cycle( out );

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

        // update segmentation registers
        for (size_t stage_id = 0; stage_id < STAGE_COUNT - 1; ++stage_id)
        {
            cpu_stages.at(stage_id + 1)->set_seg_reg(cpu_stages.at(stage_id)->get_next_seg_reg());
        }

        return ready;
    }

    void CpuFlex::reset(bool reset_data_memory, bool reset_text_memory) {
        Cpu::reset(reset_data_memory, reset_text_memory);

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

    std::vector<uint32_t> CpuFlex::get_loaded_instructions() {
        return static_cast<StageIF*>(cpu_stages.at(STAGE_IF))->get_loaded_instructions();
    }

} //namespace