#include "stage_ex.h"
#include "../hardware_manager.h"
#include "../../utils.h"
#include "../../exception.h"

#include <cassert>
#include <iostream>

using namespace std;

namespace mips_sim {

    StageEX:: StageEX(int mult_delay, int div_delay, std::shared_ptr<ControlUnit> control_unit,
            std::shared_ptr<HardwareManager> hardware_manager, std::initializer_list<signal_t> cpu_signals, std::shared_ptr<ForwardingUnit> fu,
            std::shared_ptr<SpecialRegistersBank> p_sr_bank, std::shared_ptr<GPRegistersBank> p_gpr_bank)
        : CpuStage { "EX", control_unit, hardware_manager, cpu_signals, nullptr, fu}, sr_bank { p_sr_bank }, gpr_bank { p_gpr_bank }
    {
        alu = unique_ptr<Alu>(new Alu(mult_delay, div_delay));

        sigmask = control_unit->get_signal_bitmask_static(cpu_signals);
    };

    int StageEX::work_l() {
        if (!write_segmentation_register(tmp_seg_reg))
        {
            /*TODO: STRUCTURAL HAZARD! */
            //assert(0); // sigabort??
        }
        return 0;
    };

    int StageEX::work_h() {
        // reset wrflag
        seg_reg_wrflag = false;

        if (hardware_manager->get_fp_stall()){
            return 0;
        }

        //reset tmp_seg_reg
        tmp_seg_reg = {};

        uint32_t microinstruction = seg_reg->data[SR_SIGNALS];

        /* get data from previous stage */
        uint32_t instruction_code = seg_reg->data[SR_INSTRUCTION];
        uint32_t pc_value = seg_reg->data[SR_PC];
        uint32_t rs_value = seg_reg->data[SR_RSVALUE];
        uint32_t rt_value = seg_reg->data[SR_RTVALUE];
        uint32_t addr_i   = seg_reg->data[SR_ADDR_I];
        uint32_t rs       = seg_reg->data[SR_RS];
        uint32_t rt       = seg_reg->data[SR_RT];
        uint32_t opcode   = seg_reg->data[SR_OPCODE];
        uint32_t funct    = seg_reg->data[SR_FUNCT];
        uint32_t shamt    = seg_reg->data[SR_SHAMT];

        uint32_t addr_i32 = static_cast<uint32_t>(static_cast<int>(addr_i) << 16 >> 16);

        /* temporary data */
        uint32_t hi_reg = sr_bank->get(SPECIAL_HI);
        uint32_t lo_reg = sr_bank->get(SPECIAL_LO);
        int stall_cycles = 0;
        bool hi_lo_updated = false;

        cout << "EX stage: " << Utils::decode_instruction(instruction_code) << endl;
        hardware_manager->set_status(STAGE_EX, pc_value - 4);

        uint32_t alu_input_a, alu_input_b, alu_output = UNDEF32;

        hardware_manager->add_instruction_signal(STAGE_EX, "RS", rs);
        hardware_manager->add_instruction_signal(STAGE_EX, "RT", rt);
        hardware_manager->add_instruction_signal(STAGE_EX, "ADDR_I", addr_i32);

        /* forwarding unit */
        if (fu->is_enabled())
        {   
            uint32_t rt_value;
            rs_value = fu->forward_register(rs, rs_value, false, std::cout);
            int rs_forward = fu->get_forwarded_from();
            hardware_manager->add_instruction_signal(STAGE_EX, "RS_FU", rs_forward);
            if (control_unit->test(microinstruction, SIG_REGDST) == 1 ||
                !control_unit->test(microinstruction, SIG_REGWRITE)) {
                rt_value = fu->forward_register(rt, rt_value, opcode == OP_SWC1, std::cout);
                int rt_forward = fu->get_forwarded_from();
                hardware_manager->add_instruction_signal(STAGE_EX, "RT_FU", rt_forward);
            }
        }
        uint32_t alu_src =  control_unit->test(microinstruction, SIG_ALUSRC);
        hardware_manager->add_instruction_signal(STAGE_EX, "ALU_SRC", alu_src);
        alu_input_a = rs_value;
        alu_input_b = alu_src
                    ? addr_i32
                    : rt_value;
        hardware_manager->add_instruction_signal(STAGE_EX, "ALU_A", alu_input_a);
        hardware_manager->add_instruction_signal(STAGE_EX, "ALU_B", alu_input_b);
        try
        {
            uint32_t alu_op = control_unit->test(microinstruction, SIG_ALUOP);
            hardware_manager->add_instruction_signal(STAGE_EX, "ALU_OP", alu_op);
            switch (alu_op)
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
         if (e == SYSCALL_EXCEPTION)
            syscall(gpr_bank->get("$v0"));
        else
            throw Exception::e(e, err_msg, err_v);
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

        if (hardware_manager->get_branch_stage() == STAGE_MEM && control_unit->test(microinstruction, SIG_BRANCH)) {
            hardware_manager->add_instruction_signal(STAGE_EX, "RELBRANCH", 1);
        }
        else {
            hardware_manager->add_instruction_signal(STAGE_EX, "RELBRANCH", 0);
        }

        hardware_manager->set_stage_instruction(STAGE_EX, instruction_code);

        uint32_t mem_write = control_unit->test(microinstruction, SIG_MEMWRITE);
        hardware_manager->add_instruction_signal(STAGE_EX, "MEM_WRITE", mem_write);

        hardware_manager->add_instruction_signal(STAGE_EX, "RT_VALUE", rt_value);

        return 0;
    }

    int StageEX::rising_flank() {
        return 0;
    }

    int StageEX::next_cycle() {
        return 0;
    }

    int StageEX::reset() {
        return 0;
    }

    void StageEX::set_syscall(std::function<void( uint32_t )> p_syscall) {
        syscall = p_syscall;
    }

} //namespace