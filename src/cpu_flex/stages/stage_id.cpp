#include "stage_id.h"
#include "../hardware_manager.h"
#include "../../utils.h"

#include <iostream>
#include <cassert>

using namespace std;

namespace mips_sim {

    StageID::StageID(std::shared_ptr<ControlUnit> control_unit, std::shared_ptr<HardwareManager> hardware_manager,
                    std::initializer_list<signal_t> cpu_signals, std::shared_ptr<HazardDetectionUnit> hdu, std::shared_ptr<ForwardingUnit> fu, 
                    std::shared_ptr<GPRegistersBank> p_gpr_bank, std::shared_ptr<FPRegistersBank> p_fpr_bank, 
                    std::shared_ptr<SpecialRegistersBank> p_sr_bank)
        : CpuStage { "ID", control_unit, hardware_manager, cpu_signals, hdu, fu }, gpr_bank { p_gpr_bank }, fpr_bank { p_fpr_bank }, sr_bank { p_sr_bank }
    {
        sigmask = control_unit->get_signal_bitmask_static(cpu_signals);

        //reset cp1 flag
        fp_unit_type = -1;

        status_update();
    };

    // IBranchStage
    uint32_t StageID::get_sig_pcsrc() const {
        return sig_pcsrc;
    }

    uint32_t StageID::get_addr_cbranch() const {
        return addr_cbranch;
    }

    uint32_t StageID::get_addr_rbranch() const {
        return addr_rbranch;
    }

    uint32_t StageID::get_addr_jbranch() const {
        return addr_jbranch;
    }

    //Stage ID

    bool StageID::process_branch(instruction_t instruction, uint32_t rs_value, uint32_t rt_value, uint32_t pc_value)
    {
        uint32_t opcode = instruction.opcode;
        uint32_t funct = instruction.funct;
        uint32_t addr_i32 = static_cast<uint32_t>(static_cast<int>(instruction.addr_i) << 16 >> 16);

        bool conditional_branch = (hardware_manager->get_branch_stage() == STAGE_ID) &&
                                ((rs_value == rt_value && opcode == OP_BEQ)
                            || (rs_value != rt_value && opcode == OP_BNE));

        addr_cbranch = pc_value + (addr_i32 << 2);
        addr_rbranch = rs_value;
        addr_jbranch = (pc_value & 0xF0000000) | (instruction.addr_j << 2);

        hardware_manager->add_instruction_signal(STAGE_ID, "C_BRANCH", addr_cbranch);
        hardware_manager->add_instruction_signal(STAGE_ID, "R_BRANCH", addr_rbranch);
        hardware_manager->add_instruction_signal(STAGE_ID, "J_BRANCH", addr_jbranch);

        bool branch_taken = conditional_branch
            || opcode == OP_J || opcode == OP_JAL
            || (opcode == OP_RTYPE && (funct == SUBOP_JR || funct == SUBOP_JALR));

        return branch_taken;
    }


    uint32_t StageID::read_register(uint8_t reg_index) {
        assert(reg_index < 32);
        return gpr_bank->at(reg_index);
    }

    uint32_t StageID::read_fp_register(uint8_t reg_index) {
        assert(reg_index < 32);

        return fpr_bank->at(reg_index);
    }

    int StageID::work_l() {
        write_segmentation_register(tmp_seg_reg);

        return 0;
    };

    int StageID::work_h() {
        // reset wrflag
        seg_reg_wrflag = false;

        if (hardware_manager->get_fp_stall()){
            return 0;
        }
        
        // reset pipeline_flush
        pipeline_flush_signal = 0;
        //reset cp1 flag
        fp_unit_type = -1;
        //reset tmp_seg_reg
        tmp_seg_reg = {};
        
        size_t mi_index = UNDEF32;

        /* get data from previous stage */
        uint32_t instruction_code = seg_reg->data[SR_INSTRUCTION];
        uint32_t pc_value = seg_reg->data[SR_PC];

        cout << "ID stage: " << Utils::decode_instruction(instruction_code) << endl;

        instruction_t instruction = Utils::fill_instruction(instruction_code);
        
        std::cout << "  -Instruction:"
        << " OP=" << static_cast<uint32_t>(instruction.opcode);
        if (instruction.fp_op || instruction.opcode == OP_SWC1 || instruction.opcode == OP_LWC1) {
            std::cout << " Rs=" << Utils::get_fp_register_name(instruction.rs)
            << " Rt=" << Utils::get_fp_register_name(instruction.rt)
            << " Rd=" << Utils::get_fp_register_name(instruction.rd);
        }
        else {
           std::cout  << " Rs=" << Utils::get_register_name(instruction.rs)
            << " Rt=" << Utils::get_register_name(instruction.rt)
            << " Rd=" << Utils::get_register_name(instruction.rd);
        }
        std::cout << " Shamt=" << static_cast<uint32_t>(instruction.shamt)
        << " Func=" << static_cast<uint32_t>(instruction.funct)
        << endl << "            "
        << " addr16=0x" << Utils::hex32(static_cast<uint32_t>(instruction.addr_i), 4)
        << " addr26=0x" << Utils::hex32(static_cast<uint32_t>(instruction.addr_j), 7)
        << endl;

        hardware_manager->set_status(STAGE_ID, pc_value - 4);
        hardware_manager->add_instruction_signal(STAGE_ID, "PC", pc_value);

        uint32_t microinstruction {};
        if (instruction.code == 0)
        {
            /* NOP */
            microinstruction = 0;
        }

        else
        {
            mi_index = control_unit->get_next_microinstruction_index(UNDEF32,
                                                                instruction.opcode,
                                                                instruction.funct);


            microinstruction = control_unit->get_microinstruction(mi_index);
            cout << "   Microinstruction: [" << mi_index << "]: 0x"
                << Utils::hex32(microinstruction) << endl;
        }

        bool stall = false;

        if (instruction.opcode == 0 && instruction.funct == SUBOP_SYSCALL)
        {
            //stalls until previous instructions finished
            stall = !(hardware_manager->get_stage_instruction(STAGE_EX) == 0 &&
                        hardware_manager->get_stage_instruction(STAGE_MEM) == 0);
        }

        if (instruction.opcode == OP_FTYPE && instruction.cop != 8) // BC1T and BC1F wont go to CP1
        {
            /* will go to coprocessor */
            //if rd || rt % 2 != -> Exception?

            //check if theres free funits
            switch (instruction.funct)
            {
            case SUBOP_FPADD:
            case SUBOP_FPSUB:
                //hardware_manager->is_cp1_unit_available??
                fp_unit_type = 0;
                break;
            case SUBOP_FPMUL:
                fp_unit_type = 1;
                break;
            case SUBOP_FPDIV:
                fp_unit_type = 2;
                break;
            case SUBOP_FPMOV:
            case SUBOP_FPCEQ:
            case SUBOP_FPCLE:
            case SUBOP_FPCLT:
                //no FU needed
                fp_unit_type = 3;
                //check for dependencies
                break;
            default:
                break;
            }
            

            if (hdu->is_enabled())
            {
                bool can_forward = true; //no branching here
                
                //TODO: Detect hazard
            }

            
            /* send data to next stage */
            tmp_seg_reg.data[SR_INSTRUCTION] = instruction_code;
            tmp_seg_reg.data[SR_SIGNALS] = microinstruction & sigmask;
            tmp_seg_reg.data[SR_PC]      = pc_value; // bypass PC
            tmp_seg_reg.data[SR_RSVALUE] = read_fp_register(instruction.rs);
            tmp_seg_reg.data[SR_RTVALUE] = read_fp_register(instruction.rt);
            //Read paired FPU registers
            tmp_seg_reg.data[SR_FPRSVALUEUPPER] = read_fp_register(instruction.rs + 1);
            tmp_seg_reg.data[SR_FPRTVALUEUPPER] = read_fp_register(instruction.rt + 1);
            //set precision
            tmp_seg_reg.data[SR_FPPRECISION] = (instruction.cop != 0);
            tmp_seg_reg.data[SR_ADDR_I]  = instruction.addr_i;
            tmp_seg_reg.data[SR_RT]      = instruction.rt;
            tmp_seg_reg.data[SR_RD]      = instruction.rd;
            tmp_seg_reg.data[SR_REGDEST] = instruction.rd;
            tmp_seg_reg.data[SR_FUNCT]   = instruction.funct;
            tmp_seg_reg.data[SR_OPCODE]  = instruction.opcode;
            tmp_seg_reg.data[SR_RS]      = instruction.rs;
            tmp_seg_reg.data[SR_SHAMT]   = instruction.shamt;
            tmp_seg_reg.data[SR_IID]     = seg_reg->data[SR_IID];
        }
        else
        {
            /* integer unit */
            uint32_t rs_value = read_register(instruction.rs);
            uint32_t rt_value;
            if (control_unit->test(microinstruction, SIG_REGBANK))
                rt_value = read_fp_register(instruction.rt);
            else
                rt_value = read_register(instruction.rt);
            
            // get data for GUI
            hardware_manager->add_instruction_signal(STAGE_ID, "RS_REG", instruction.rs);
            hardware_manager->add_instruction_signal(STAGE_ID, "RS_VALUE", rs_value);
            hardware_manager->add_instruction_signal(STAGE_ID, "RT_REG", instruction.rt);
            hardware_manager->add_instruction_signal(STAGE_ID, "RT_VALUE", rt_value);
            // data EX will use
            uint32_t alu_src =  control_unit->test(microinstruction, SIG_ALUSRC);
            hardware_manager->add_instruction_signal(STAGE_ID, "ALU_SRC", alu_src);
            hardware_manager->add_instruction_signal(STAGE_ID, "IMM_VALUE", instruction.addr_i);
            // data WB will use
            uint32_t reg_write = control_unit->test(microinstruction, SIG_REGWRITE);
            hardware_manager->add_instruction_signal(STAGE_ID, "REG_WRITE", reg_write);

            if (hdu->is_enabled())
            {
                //TODO: Branch stage can be decided using additional signals.
                // That way we don't need explicit comparisons here
                bool can_forward = fu->is_enabled() &&
                                ((instruction.opcode != OP_BNE && instruction.opcode != OP_BEQ && instruction.cop != 8) //added check for BC1T/F here
                                    || hardware_manager->get_branch_stage() > STAGE_ID);

                // check for hazards
                if (instruction.code > 0 && instruction.funct != SUBOP_SYSCALL
                    && instruction.opcode != OP_LUI)
                {
                    stall = hdu->detect_hazard(instruction.rs, can_forward);

                    if ((!control_unit->test(microinstruction, SIG_ALUSRC))
                        || instruction.opcode == OP_SW
                        || instruction.opcode == OP_SWC1)
                    {
                        stall |= hdu->detect_hazard(instruction.rt, can_forward, instruction.opcode == OP_SWC1);
                    }
                }

                if (stall) cout << "   Hazard detected: Pipeline stall" << endl;
            } 
            hardware_manager->add_instruction_signal(STAGE_ID, "STALL", stall);
            pc_write = !stall;
            if (stall)
            {
                // send "NOP" to next stage 
                tmp_seg_reg = {};
            }
            else
            {
                if (control_unit->test(microinstruction, SIG_BRANCH))
                {
                    // unconditional branches are resolved here 
                    bool branch_taken = process_branch(instruction,
                                                    rs_value, rt_value, pc_value);

                    if (!branch_taken)
                    {
                        control_unit->set(microinstruction, SIG_PCSRC, 0);
                    }
                    else {
                        if (hardware_manager->get_branch_stage() == STAGE_ID) {
                            hardware_manager->add_instruction_signal(STAGE_ID, "BRANCH", 1);
                        }
                    }

                    if (hardware_manager->get_branch_type() == BRANCH_FLUSH
                        || (hardware_manager->get_branch_type() == BRANCH_NON_TAKEN && branch_taken))
                    {
                        pipeline_flush_signal = 1;

                        if (!branch_taken)
                            sr_bank->set("pc", pc_value - 4);
                    }
                }
                
                else {
                    hardware_manager->add_instruction_signal(STAGE_ID, "BRANCH", 0);
                }

                uint32_t reg_dest;
                uint32_t sig_reg_dest = control_unit->test(microinstruction, SIG_REGDST);
                hardware_manager->add_instruction_signal(STAGE_ID, "REG_DEST", sig_reg_dest);
                switch(sig_reg_dest)
                {
                    case 0:
                        reg_dest = instruction.rt; break;
                    case 1:
                        reg_dest = instruction.rd; break;
                    case 2:
                        reg_dest = 31; break;
                    default:
                        assert(0);
                }

                hardware_manager->add_instruction_signal(STAGE_ID, "REG_DEST_REGISTER", reg_dest);

                /* send data to next stage */
                tmp_seg_reg.data[SR_INSTRUCTION] = instruction_code;
                tmp_seg_reg.data[SR_SIGNALS] = microinstruction & sigmask;
                tmp_seg_reg.data[SR_PC]      = pc_value; // bypass PC
                tmp_seg_reg.data[SR_RSVALUE] = rs_value;
                tmp_seg_reg.data[SR_RTVALUE] = rt_value;
                tmp_seg_reg.data[SR_ADDR_I]  = instruction.addr_i;
                tmp_seg_reg.data[SR_RT]      = instruction.rt;
                tmp_seg_reg.data[SR_RD]      = instruction.rd;
                tmp_seg_reg.data[SR_REGDEST] = reg_dest;
                tmp_seg_reg.data[SR_FUNCT]   = instruction.funct;
                tmp_seg_reg.data[SR_OPCODE]  = instruction.opcode;
                tmp_seg_reg.data[SR_RS]      = instruction.rs;
                tmp_seg_reg.data[SR_SHAMT]   = instruction.shamt;
                tmp_seg_reg.data[SR_IID]     = seg_reg->data[SR_IID];

                sig_pcsrc = control_unit->test(microinstruction & sigmask, SIG_PCSRC);

                hardware_manager->add_instruction_signal(STAGE_ID, "INSTRUCTION", instruction_code);
       
            }
        }

        return 0;
    }

    int StageID::rising_flank() {
        return 0;
    }

    int StageID::next_cycle() {
        return 0;
    }

    int StageID::reset() {
        pc_write = true;

        addr_cbranch = 0;
        addr_jbranch = 0;
        addr_rbranch = 0;

        pipeline_flush_signal = 0;

        fp_unit_type = -1;

        return 0;
    }

    bool StageID::get_pc_write() {
        return pc_write;
    }

    void StageID::status_update()
    {   
        //pc_write is always in ID
        hardware_manager->set_signal(SIGNAL_PCWRITE, bind(&StageID::get_pc_write, this));

        /* bind functions */
        if (hardware_manager->get_branch_stage() == STAGE_ID)
            {
            hardware_manager->set_signal(SIGNAL_PCSRC,   bind(&StageID::get_sig_pcsrc, this));
            hardware_manager->set_signal(SIGNAL_CBRANCH, bind(&StageID::get_addr_cbranch, this));
            hardware_manager->set_signal(SIGNAL_RBRANCH, bind(&StageID::get_addr_rbranch, this));
            hardware_manager->set_signal(SIGNAL_JBRANCH, bind(&StageID::get_addr_jbranch, this));
            hardware_manager->set_signal(SIGNAL_FLUSH,   bind(&StageID::get_pipeline_flush_signal, this));
        }
    }

    int StageID::send_to_cp1() {
        return fp_unit_type;
    }

    uint32_t StageID::get_pipeline_flush_signal() const {
        return pipeline_flush_signal;
    }

} //namespace