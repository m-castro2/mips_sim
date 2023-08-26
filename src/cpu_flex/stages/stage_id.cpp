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

        status_update();
    };

    // IBranchStage
    uint32_t StageID::get_sig_pcsrc() const {
        cout << "Stage " << stage_name << " get_sigpcsrc\n";
        return sig_pcsrc;
    }

    uint32_t StageID::get_addr_cbranch() const {
        cout << "Stage " << stage_name << " get_addr_cbranch\n";
        return addr_cbranch;
    }

    uint32_t StageID::get_addr_rbranch() const {
        cout << "Stage " << stage_name << " get_addr_rbranch\n";
        return addr_rbranch;
    }

    uint32_t StageID::get_addr_jbranch() const {
        cout << "Stage " << stage_name << " get_addr_jbranch\n";
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
        cout << "Stage " << stage_name << " work_l\n" ;

        bool stall = false;

        sig_pcsrc = control_unit->test(microinstruction & sigmask, SIG_PCSRC); //why does bne break if written in work_l ??

        /* if (instruction.opcode == 0 && instruction.funct == SUBOP_SYSCALL)
        {
            //stalls until previous instructions finished
            stall = !(seg_regs[ID_EX].data[SR_INSTRUCTION] == 0 &&
                        seg_regs[EX_MEM].data[SR_INSTRUCTION] == 0);
        } */

        if (instruction.opcode == OP_FTYPE)
        {
            /* will go to coprocessor */
            //TODO
            //throw Exception::e(CPU_UNIMPL_EXCEPTION, "Instruction not implemented yet: " + Utils::decode_instruction(instruction_code));

            //write_segmentation_register(ID_EX, {});
        }
        else
        {
            /* integer unit */
            rs_value = read_register(instruction.rs);
            if (control_unit->test(microinstruction, SIG_REGBANK))
                rt_value = read_fp_register(instruction.rt);
            else
                rt_value = read_register(instruction.rt);

            if (hdu->is_enabled())
            {
                //TODO: Branch stage can be decided using additional signals.
                // That way we don't need explicit comparisons here
                bool can_forward = fu->is_enabled() &&
                                ((instruction.opcode != OP_BNE && instruction.opcode != OP_BEQ)
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

            pc_write = !stall;
            if (stall)
            {
                // send "NOP" to next stage 
                next_seg_reg = {};
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

                    if (hardware_manager->get_branch_type() == BRANCH_FLUSH
                        || (hardware_manager->get_branch_type() == BRANCH_NON_TAKEN && branch_taken))
                    {
                        pipeline_flush_signal = 1;

                        if (!branch_taken)
                            sr_bank->set("pc", pc_value-4);
                    }
                }

                switch(control_unit->test(microinstruction, SIG_REGDST))
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
       
            }
        }

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

        /* for (int i = 0; i < 32; ++i) {
            std::cout << "\t\tTMP Seg reg " << i << ": "  << Utils::hex32(tmp_seg_reg.data[i]) << endl;
        } */

        write_segmentation_register(tmp_seg_reg);

        return 0;
    };

    int StageID::work_h() {
        cout << "Stage " << stage_name << " work_h\n";

        // reset wrflag
        seg_reg_wrflag = false;
        // reset pipeline_flush
        pipeline_flush_signal = 0;
        
        size_t mi_index = UNDEF32;

        /* get data from previous stage */
        instruction_code = seg_reg->data[SR_INSTRUCTION];
        pc_value = seg_reg->data[SR_PC];

        instruction = Utils::fill_instruction(instruction_code);

        std::cout << "  -Instruction:"
        << " OP=" << static_cast<uint32_t>(instruction.opcode)
        << " Rs=" << Utils::get_register_name(instruction.rs)
        << " Rt=" << Utils::get_register_name(instruction.rt)
        << " Rd=" << Utils::get_register_name(instruction.rd)
        << " Shamt=" << static_cast<uint32_t>(instruction.shamt)
        << " Func=" << static_cast<uint32_t>(instruction.funct)
        << endl << "            "
        << " addr16=0x" << Utils::hex32(static_cast<uint32_t>(instruction.addr_i), 4)
        << " addr26=0x" << Utils::hex32(static_cast<uint32_t>(instruction.addr_j), 7)
        << endl;

        hardware_manager->set_status(STAGE_ID, pc_value - 4);

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

        return 0;
    }

    int StageID::next_cycle() {
        work_h();
        work_l();
        return 0;
    }

    int StageID::reset() {
        cout << "Stage " << stage_name << " reset\n";

        pc_write = true;

        addr_cbranch = 0;
        

        return 0;
    }

    bool StageID::get_pc_write() {
        return pc_write;
    }

    void StageID::status_update()
    {
        /* bind functions */
        if (hardware_manager->get_branch_stage() == STAGE_ID)
            {
            hardware_manager->set_signal(SIGNAL_PCSRC,   bind(&StageID::get_sig_pcsrc, this));
            hardware_manager->set_signal(SIGNAL_CBRANCH, bind(&StageID::get_addr_cbranch, this));
            hardware_manager->set_signal(SIGNAL_RBRANCH, bind(&StageID::get_addr_rbranch, this));
            hardware_manager->set_signal(SIGNAL_JBRANCH, bind(&StageID::get_addr_jbranch, this));
            hardware_manager->set_signal(SIGNAL_FLUSH,   bind(&StageID::pipeline_flush_signal, this));
            hardware_manager->set_signal(SIGNAL_PCWRITE,   bind(&StageID::get_pc_write, this));
        }
    }

} //namespace