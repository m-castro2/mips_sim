#include "stage_fwb.h"
#include "../hardware_manager.h"
#include "../../utils.h"

#include <cassert>
#include <iostream>

using namespace std;

namespace mips_sim {

    StageFWB::StageFWB(std::shared_ptr<ControlUnit> control_unit, std::shared_ptr<HardwareManager> hardware_manager,
        std::shared_ptr<GPRegistersBank> p_gpr_bank, std::shared_ptr<FPRegistersBank> p_fpr_bank)
        : CpuStage { "WB", control_unit, hardware_manager },  gpr_bank { p_gpr_bank }, fpr_bank { p_fpr_bank }
    {

    };

    int StageFWB::work_l() {

        return 0;
    };

    int StageFWB::work_h() {

        return 0;
    }

    int StageFWB::rising_flank() {
        // reset wrflag
        seg_reg_wrflag = false;
        //reset tmp_seg_reg
        tmp_seg_reg = {};

        uint32_t regwrite_value = UNDEF32;

        /* get data from previous stage */
        uint32_t instruction_code = seg_reg->data[SR_INSTRUCTION];
        uint32_t pc_value         = seg_reg->data[SR_PC];
        uint32_t microinstruction = seg_reg->data[SR_SIGNALS];
        uint32_t reg_dest         = seg_reg->data[SR_REGDEST];
        uint32_t mem_word_read    = seg_reg->data[SR_WORDREAD];
        uint32_t alu_output       = seg_reg->data[SR_ALUOUTPUT];

        if (!control_unit->test(microinstruction, SIG_REGBANK) && (microinstruction != 0)) {
            std::cout << "FWB Stage: " << Utils::decode_instruction(instruction_code) << endl;
            std::cout << "\t No FPRegister involved, continue" << endl;
            return 0;
        }

        std::cout << "FWB Stage: " << Utils::decode_instruction(instruction_code) << endl;
        //current_state[STAGE_WB] = pc_value-4;

        switch (control_unit->test(microinstruction, SIG_MEM2REG))
        {
        case 0:
            regwrite_value = mem_word_read; break;
        case 1:
            regwrite_value = alu_output; break;
        case 2:
            regwrite_value = pc_value; break;
        default:
            assert(0);
        }

        if (control_unit->test(microinstruction, SIG_REGWRITE))
        {
            std::cout << "   Result value: 0x" << Utils::hex32(regwrite_value) << endl;
            if (control_unit->test(microinstruction, SIG_REGBANK))
                std::cout << "   Register dest: " << Utils::get_fp_register_name(reg_dest) << endl;
            else
                std::cout << "   Register dest: " << Utils::get_register_name(reg_dest) << endl;
            std::cout << "   Signal Mem2Reg: " << control_unit->test(microinstruction, SIG_MEM2REG) << endl;
        }

        if (control_unit->test(microinstruction, SIG_REGWRITE))
        {
            if (control_unit->test(microinstruction, SIG_REGBANK))
            {
                assert(reg_dest < 32);
                uint8_t reg_dest8 = static_cast<uint8_t>(reg_dest);

                std::cout << "   REG write " << Utils::get_fp_register_name(reg_dest8)
                    << " <-- 0x" << Utils::hex32(regwrite_value) << endl;
                write_fp_register(reg_dest8, regwrite_value);
            }
            else
            {
                assert(reg_dest < 32);
                uint8_t reg_dest8 = static_cast<uint8_t>(reg_dest);
                
                std::cout << "   REG write " << Utils::get_register_name(reg_dest8)
                    << " <-- 0x" << Utils::hex32(regwrite_value) << endl;
                write_register(reg_dest8, regwrite_value);
            }
        }

        return 0;
    }

    int StageFWB::next_cycle() {
        work_h();
        work_l();
        return 0;
    }

    int StageFWB::reset() {
        return 0;
    }

    void StageFWB::write_register( uint8_t reg_index, uint32_t value)
    {
        /* if (reg_index == 0)
            throw Exception::e(CPU_REG_EXCEPTION, "Cannot write register $0"); */
        assert(reg_index < 32);

        gpr_bank->set_at(reg_index, value);
    }

    void StageFWB::write_fp_register( uint8_t reg_index, uint32_t value)
        {
        assert(reg_index < 32);

        fpr_bank->set_at(reg_index, value);
    }

    // IForwardableStage
    bool StageFWB::forward_register(int regId, int regValue) {
        return 0;
    }


} //namespace