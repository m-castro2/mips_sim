#include "stage_wb.h"
#include "../hardware_manager.h"
#include "../../utils.h"

#include <cassert>
#include <iostream>

using namespace std;

namespace mips_sim {

    StageWB::StageWB(std::shared_ptr<ControlUnit> control_unit, std::shared_ptr<HardwareManager> hardware_manager,
        std::shared_ptr<GPRegistersBank> p_gpr_bank, std::shared_ptr<FPRegistersBank> p_fpr_bank)
        : CpuStage { "WB", control_unit, hardware_manager },  gpr_bank { p_gpr_bank }, fpr_bank { p_fpr_bank }
    {

    };

    int StageWB::work_l() {

        return 0;
    };

    int StageWB::work_h() {
        if (!write_segmentation_register(tmp_seg_reg))
        {
            /* no structural hazard should happen here */
            assert(0);
        }
        return 0;
    }

    int StageWB::rising_flank() {
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
        uint32_t fp_output_upper  = seg_reg->data[SR_FPOUTPUTUPPER];

        //if FPRegister, send data to FWB stage
        if (control_unit->test(microinstruction, SIG_REGBANK) || (seg_reg->data[SR_OPCODE] == OP_FTYPE)) {
            tmp_seg_reg.data[SR_INSTRUCTION] = instruction_code;
            tmp_seg_reg.data[SR_PC] = pc_value;
            tmp_seg_reg.data[SR_SIGNALS] = microinstruction;
            tmp_seg_reg.data[SR_REGDEST] = reg_dest;
            tmp_seg_reg.data[SR_WORDREAD] = mem_word_read;
            tmp_seg_reg.data[SR_ALUOUTPUT] = alu_output;
            tmp_seg_reg.data[SR_OPCODE] = seg_reg->data[SR_OPCODE];
            tmp_seg_reg.data[SR_FPOUTPUTUPPER] = fp_output_upper;
            tmp_seg_reg.data[SR_FPPRECISION] = seg_reg->data[SR_FPPRECISION];

            std::cout << "WB Stage: " << Utils::decode_instruction(instruction_code) << endl;
            std::cout << "\t FPRegister, redirect to FWB Stage" << endl;
            return 0;
        }

        std::cout << "WB Stage: " << Utils::decode_instruction(instruction_code) << endl;
        //current_state[STAGE_WB] = pc_value-4;

        uint32_t mem2reg = control_unit->test(microinstruction, SIG_MEM2REG);
        hardware_manager->add_instruction_signal(STAGE_WB, "MEM_2_REG", mem2reg);
        switch (mem2reg)
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

        uint32_t reg_write = control_unit->test(microinstruction, SIG_REGWRITE);
        hardware_manager->add_instruction_signal(STAGE_WB, "REG_WRITE", reg_write);
        if (reg_write)
        {
            std::cout << "   Result value: 0x" << Utils::hex32(regwrite_value) << endl;
            std::cout << "   Register dest: " << Utils::get_register_name(reg_dest) << endl;
            std::cout << "   Signal Mem2Reg: " << control_unit->test(microinstruction, SIG_MEM2REG) << endl;

            assert(reg_dest < 32);
            uint8_t reg_dest8 = static_cast<uint8_t>(reg_dest);
            
            std::cout << "   REG write " << Utils::get_register_name(reg_dest8)
                << " <-- 0x" << Utils::hex32(regwrite_value) << endl;
            write_register(reg_dest8, regwrite_value);
    
        }

        // FU values
        hardware_manager->add_instruction_signal(STAGE_WB, "REG_DEST_REGISTER", reg_dest);
        hardware_manager->add_instruction_signal(STAGE_WB, "REG_VALUE", regwrite_value);

        hardware_manager->add_instruction_signal(STAGE_WB, "PC", pc_value);

        hardware_manager->set_stage_instruction(STAGE_WB, instruction_code);

        return 0;
    }

    int StageWB::next_cycle() {
        work_h();
        work_l();
        return 0;
    }

    int StageWB::reset() {
        return 0;
    }

    void StageWB::write_register( uint8_t reg_index, uint32_t value)
    {
        /* if (reg_index == 0)
            throw Exception::e(CPU_REG_EXCEPTION, "Cannot write register $0"); */
        assert(reg_index < 32);

        gpr_bank->set_at(reg_index, value);
    }

    void StageWB::write_fp_register( uint8_t reg_index, uint32_t value)
        {
        assert(reg_index < 32);

        fpr_bank->set_at(reg_index, value);
    }

    // IForwardableStage
    bool StageWB::forward_register(int regId, int regValue) {
        return 0;
    }


} //namespace