#include "stage_ex.h"
#include "../hardware_manager.h"
#include "../../utils.h"

#include <cassert>
#include <iostream>

using namespace std;

namespace mips_sim {

    StageEX:: StageEX(int mult_delay, int div_delay, std::shared_ptr<ControlUnit> control_unit,
            std::shared_ptr<HardwareManager> hardware_manager, std::initializer_list<signal_t> cpu_signals, std::shared_ptr<ForwardingUnit> fu,
            std::shared_ptr<SpecialRegistersBank> p_sr_bank)
        : CpuStage { "EX", control_unit, hardware_manager, cpu_signals, nullptr, fu}, sr_bank { p_sr_bank }
    {
        alu = unique_ptr<Alu>(new Alu(mult_delay, div_delay));
    };

    int StageEX::work_l() {
        cout << "Stage " << stage_name << " work_l\n" ;

        /* send data to next stage */
        tmp_seg_reg.data[SR_INSTRUCTION] = instruction_code;
        tmp_seg_reg.data[SR_PC]         = pc_value; /* bypass PC */
        tmp_seg_reg.data[SR_SIGNALS]    = microinstruction & sigmask;
        tmp_seg_reg.data[SR_RELBRANCH]  = (addr_i32 << 2) + seg_reg->data[SR_PC];
        tmp_seg_reg.data[SR_ALUZERO]    = ((alu_output == 0) && (opcode == OP_BEQ))
                                                || ((alu_output != 0) && (opcode == OP_BNE));
        tmp_seg_reg.data[SR_ALUOUTPUT]  = alu_output;
        tmp_seg_reg.data[SR_RTVALUE]    = rt_value;
        tmp_seg_reg.data[SR_REGDEST]    = seg_reg->data[SR_REGDEST];

        tmp_seg_reg.data[SR_IID] = seg_reg->data[SR_IID];

        if (!write_segmentation_register(tmp_seg_reg))
        {
            /*TODO: STRUCTURAL HAZARD! */
            //assert(0); // sigabort??
        }

        return 0;
    };

    int StageEX::work_h() {
        cout << "Stage " << stage_name << " work_h\n";

        // reset wrflag
        seg_reg_wrflag = false;

        microinstruction = seg_reg->data[SR_SIGNALS];
        uint32_t alu_input_a, alu_input_b = UNDEF32;

        /* get data from previous stage */
        instruction_code = seg_reg->data[SR_INSTRUCTION];
        pc_value = seg_reg->data[SR_PC];
        uint32_t rs_value = seg_reg->data[SR_RSVALUE];
        rt_value = seg_reg->data[SR_RTVALUE];
        uint32_t addr_i   = seg_reg->data[SR_ADDR_I];
        uint32_t rs       = seg_reg->data[SR_RS];
        uint32_t rt       = seg_reg->data[SR_RT];
        opcode   = seg_reg->data[SR_OPCODE];
        uint32_t funct    = seg_reg->data[SR_FUNCT];
        uint32_t shamt    = seg_reg->data[SR_SHAMT];

        addr_i32 = static_cast<uint32_t>(static_cast<int>(addr_i) << 16 >> 16);

        /* temporary data */
        hi_reg = sr_bank->get(SPECIAL_HI);
        lo_reg = sr_bank->get(SPECIAL_LO);
        stall_cycles = 0;
        hi_lo_updated = false;

        cout << "EX stage: " << Utils::decode_instruction(instruction_code) << endl;
        hardware_manager->set_status(STAGE_EX, pc_value - 4);

        /* forwarding unit */
        if (fu->is_enabled())
        {
            rs_value = fu->forward_register(rs, rs_value, false, std::cout);
            if (control_unit->test(microinstruction, SIG_REGDST) == 1 ||
                !control_unit->test(microinstruction, SIG_REGWRITE))
                rt_value = fu->forward_register(rt, rt_value, opcode == OP_SWC1, std::cout);
        }

        alu_input_a = rs_value;
        alu_input_b = control_unit->test(microinstruction, SIG_ALUSRC)
                    ? addr_i32
                    : rt_value;
        try
        {
            switch (control_unit->test(microinstruction, SIG_ALUOP))
            {
                case 0:
                    alu_output = alu->compute_subop(alu_input_a, alu_input_b,
                                                    static_cast<uint8_t>(shamt), SUBOP_ADDU,
                                                    &hi_reg, &lo_reg, &stall_cycles);
                    hi_lo_updated = true;
                    
                    break;
                case 1:
                    alu_output = alu->compute_subop(alu_input_a, alu_input_b,
                                                    static_cast<uint8_t>(shamt), SUBOP_SUBU,
                                                    &hi_reg, &lo_reg, &stall_cycles);
                    hi_lo_updated = true;
                    break;
                case 2:
                    if (opcode == OP_RTYPE)
                    {
                        alu_output = alu->compute_subop(alu_input_a, alu_input_b,
                                                    static_cast<uint8_t>(shamt), funct,
                                                    &hi_reg, &lo_reg, &stall_cycles);
                        hi_lo_updated = true;
                    }
                    else
                        alu_output = alu->compute_op(alu_input_a, alu_input_b, opcode);
                    break;
                default:
                    std::cerr << "Undefined ALU operation" << std::endl;
                    assert(0);
            }  
        }
        catch(int e)
        {
        /*  if (e == SYSCALL_EXCEPTION)
            syscall(gpr_bank->get("$v0"));
        else
            throw Exception::e(e, err_msg, err_v); */
        }

        if (hi_lo_updated)
        {
            sr_bank->set(SPECIAL_HI, hi_reg);
            sr_bank->set(SPECIAL_LO, lo_reg);
            int execution_stall = stall_cycles;
        }

        std::cout << "   ALU compute 0x" << Utils::hex32(alu_input_a) << " OP 0x"
            << Utils::hex32(alu_input_b) << " = 0x"
            << Utils::hex32(alu_output) << endl;

        return 0;
    }

    int StageEX::next_cycle() {
        work_h();
        work_l();
        return 0;
    }

    int StageEX::reset() {
        cout << "Stage " << stage_name << " reset\n";
        return 0;
    }

} //namespace