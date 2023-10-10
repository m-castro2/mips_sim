#include "stage_mem.h"
#include "../hardware_manager.h"
#include "../../utils.h"

#include <iostream>

using namespace std;

namespace mips_sim {

    StageMEM::StageMEM(std::shared_ptr<Memory> _memory, std::shared_ptr<ControlUnit> control_unit, std::shared_ptr<HardwareManager> hardware_manager,
                    std::initializer_list<signal_t> cpu_signals, std::shared_ptr<SpecialRegistersBank> p_sr_bank)
        : memory {_memory}, sr_bank { p_sr_bank }, CpuStage { "MEM", control_unit, hardware_manager, cpu_signals }
    {
        sigmask = control_unit->get_signal_bitmask_static(cpu_signals);
    };

    int StageMEM::work_l() {
        if (next_pc != 0) {
            sr_bank->set(SPECIAL_PC, branch_addr);
        }
        else {
            sig_pcsrc = 0;
        }
        return 0;
    };

    int StageMEM::work_h() {
        // reset wrflag
        seg_reg_wrflag = false;
        //reset tmp_seg_reg
        tmp_seg_reg = {};
        // reset pipeline_flush
        pipeline_flush_signal = 0;
        next_pc = 0;

        uint32_t word_read = UNDEF32;

        /* get data from previous stage */
        uint32_t instruction_code = seg_reg->data[SR_INSTRUCTION];
        uint32_t pc_value         = seg_reg->data[SR_PC];
        uint32_t microinstruction = seg_reg->data[SR_SIGNALS];
        uint32_t mem_addr         = seg_reg->data[SR_ALUOUTPUT];
        uint32_t rt_value         = seg_reg->data[SR_RTVALUE];
        branch_addr      = seg_reg->data[SR_RELBRANCH];
        uint32_t branch_taken     = seg_reg->data[SR_ALUZERO];

        hardware_manager->add_instruction_signal(STAGE_MEM, "RT_VALUE", rt_value);

        std::cout << "MEM stage: " << Utils::decode_instruction(instruction_code) << endl;
        hardware_manager->set_status(STAGE_MEM, pc_value-4);
        hardware_manager->add_instruction_signal(STAGE_MEM, "PC", pc_value - 4);

        if (hardware_manager->get_branch_stage() == STAGE_MEM && control_unit->test(microinstruction, SIG_BRANCH))
        {
            hardware_manager->add_instruction_signal(STAGE_MEM, "BRANCH", 1);
            /* if conditional branches are resolved here */
            if (hardware_manager->get_branch_type() == BRANCH_FLUSH
                || (hardware_manager->get_branch_type() == BRANCH_NON_TAKEN && branch_taken))
            {
                pipeline_flush_signal = 3;

                if (!branch_taken)
                    sr_bank->set(SPECIAL_PC, pc_value-4);
            }

            if (branch_taken)
            {
                std::cout << "  BRANCH: Jump to 0x" << Utils::hex32(branch_addr) << endl;

                next_pc = branch_addr; //redundant
                addr_cbranch = branch_addr;
                
                sig_pcsrc = 1;
            }
            else {
                sig_pcsrc = 0;
            }
        }

        else {
            hardware_manager->add_instruction_signal(STAGE_MEM, "BRANCH", 0);
        }

        if (control_unit->test(microinstruction, SIG_MEMREAD))
        {
            std::cout << "   MEM read 0x" << Utils::hex32(mem_addr);
            word_read = memory->mem_read_32(mem_addr);
            hardware_manager->add_instruction_signal(STAGE_MEM, "MEM_READ", 1);
            hardware_manager->add_instruction_signal(STAGE_MEM, "MEM_WRITE", 0);
        }
        else if (control_unit->test(microinstruction, SIG_MEMWRITE))
        {
            std::cout << "   MEM write 0x" << Utils::hex32(rt_value) << " to 0x" << Utils::hex32(mem_addr) << endl;
            memory->mem_write_32(mem_addr, rt_value);
            hardware_manager->add_instruction_signal(STAGE_MEM, "MEM_READ", 0);
            hardware_manager->add_instruction_signal(STAGE_MEM, "MEM_WRITE", 1);
        }
        else {
            hardware_manager->add_instruction_signal(STAGE_MEM, "MEM_READ", 0);
            hardware_manager->add_instruction_signal(STAGE_MEM, "MEM_WRITE", 0);
        }

        // ...
        std::cout << "   Address/ALU_Bypass: 0x" << Utils::hex32(mem_addr) << endl;


        /* send data to next stage */
        tmp_seg_reg.data[SR_INSTRUCTION] = instruction_code;
        tmp_seg_reg.data[SR_PC]          = pc_value; /* bypass PC */
        tmp_seg_reg.data[SR_SIGNALS]     = microinstruction & sigmask;
        tmp_seg_reg.data[SR_WORDREAD]    = word_read;
        tmp_seg_reg.data[SR_ALUOUTPUT]   = mem_addr; // come from alu output
        tmp_seg_reg.data[SR_REGDEST]     = seg_reg->data[SR_REGDEST];
        tmp_seg_reg.data[SR_OPCODE]      = seg_reg->data[SR_OPCODE];

        tmp_seg_reg.data[SR_IID] = seg_reg->data[SR_IID];

        hardware_manager->set_stage_instruction(STAGE_MEM, instruction_code);

        uint32_t reg_write = control_unit->test(microinstruction, SIG_REGWRITE);
        hardware_manager->add_instruction_signal(STAGE_MEM, "REG_WRITE", reg_write);

        // FU values
        uint32_t mem_regdest  = seg_reg->data[SR_REGDEST];
        hardware_manager->add_instruction_signal(STAGE_MEM, "REG_DEST_REGISTER", mem_regdest);
        hardware_manager->add_instruction_signal(STAGE_MEM, "REG_VALUE", mem_addr);

        hardware_manager->add_instruction_signal(STAGE_MEM, "MEM_OUT", word_read);

        if (!write_segmentation_register(tmp_seg_reg))
        {
            /* no structural hazard should happen here */
            assert(0);
        }

        return 0;
    }

    int StageMEM::rising_flank() {
        return 0;
    }

    int StageMEM::next_cycle() {
        return 0;
    }

    int StageMEM::reset() {
        set_seg_reg({});
        tmp_seg_reg = {};
        next_seg_reg = {};
        pipeline_flush_signal = 0;
        addr_cbranch = 0;
        addr_jbranch = 0;
        addr_rbranch = 0;
        sig_pcsrc = 0;
        return 0;
    }

    // IBranchStage
    uint32_t StageMEM::get_sig_pcsrc() const {
        return sig_pcsrc;
    }

    uint32_t StageMEM::get_addr_cbranch() const {
        return addr_cbranch;
    }

    uint32_t StageMEM::get_addr_rbranch() const {
        return addr_rbranch;
    }

    uint32_t StageMEM::get_addr_jbranch() const {
        return addr_jbranch;
    }

    // IForwardableStage
    bool StageMEM::forward_register(int regId, int regValue) {
        return 0;
    }

    void StageMEM::status_update()
    {
        /* bind functions */
        if (hardware_manager->get_branch_stage() == STAGE_MEM)
            {
            hardware_manager->set_signal(SIGNAL_PCSRC,   bind(&StageMEM::get_sig_pcsrc, this));
            hardware_manager->set_signal(SIGNAL_CBRANCH, bind(&StageMEM::get_addr_cbranch, this));
            hardware_manager->set_signal(SIGNAL_RBRANCH, bind(&StageMEM::get_addr_rbranch, this));
            hardware_manager->set_signal(SIGNAL_JBRANCH, bind(&StageMEM::get_addr_jbranch, this));
            hardware_manager->set_signal(SIGNAL_FLUSH,   bind(&StageMEM::get_pipeline_flush_signal, this));
            //hardware_manager->set_signal(SIGNAL_PCWRITE,   bind(&StageMEM::get_pc_write, this));
        }
    }

    uint32_t StageMEM::get_pipeline_flush_signal() const {
        return pipeline_flush_signal;
    }


} //namespace