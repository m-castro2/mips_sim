#include "stage_if.h"
#include "../hardware_manager.h"

#include "../../cpu/component/registers_bank.h"
#include "../../utils.h"

#include <iostream>
#include <cassert>

using namespace std;

namespace mips_sim {

    StageIF::StageIF(std::shared_ptr<Memory> _memory, std::shared_ptr<HardwareManager> hardware_manager, 
                    std::shared_ptr<ControlUnit> control_unit, std::shared_ptr<SpecialRegistersBank> _sr_bank)
        : memory { _memory }, sr_bank { _sr_bank }, CpuStage { "IF" , control_unit, hardware_manager }
    {
        loaded_instructions.push_back(sr_bank->get(SPECIAL_PC));
    };

    int StageIF::work_l() {
        cout << "Stage " << stage_name << " work_l\n" ;

        sr_bank->set(SPECIAL_PC, current_pc);
        
        /* next instruction */
        tmp_seg_reg.data[SR_INSTRUCTION] = instruction_code;
        tmp_seg_reg.data[SR_PC] = current_pc;

        tmp_seg_reg.data[SR_IID] = loaded_instruction_index;

        write_segmentation_register(tmp_seg_reg);

        return 0;
    };

    int StageIF::work_h() {
        cout << "Stage " << stage_name << " work_h\n";

        // reset wrflag
        seg_reg_wrflag = false;

        current_pc = sr_bank->get(SPECIAL_PC);

        pc_src = hardware_manager->get_signal(SIGNAL_PCSRC)();

        hardware_manager->set_status(STAGE_IF, current_pc);

        if (current_pc != loaded_instructions[loaded_instruction_index])
        {
            loaded_instruction_index++;
            loaded_instructions.push_back(current_pc);
        }

        // fetch instruction 
        instruction_code = memory->mem_read_32(current_pc);


        if (!hardware_manager->get_signal(SIGNAL_PCWRITE)()) {
            return 1;
        }

        switch(pc_src)
        {
            case 0:
                current_pc += 4; break;
            case 1:
                current_pc = hardware_manager->get_signal(SIGNAL_CBRANCH)();
                cout << "   !!! Conditional branch taken >> 0x"
                    << Utils::hex32(current_pc) << endl; break;
            case 2:
                current_pc = hardware_manager->get_signal(SIGNAL_RBRANCH)(); 
                cout << "   !!! Register jump taken >> 0x"
                    << Utils::hex32(current_pc) << endl; break;
            case 3:
                current_pc = hardware_manager->get_signal(SIGNAL_JBRANCH)();
                cout << "   !!! Unconditional jump taken >> 0x"
                    << Utils::hex32(current_pc) << endl; break;
            default:
                assert(0);
        }

        return 0;
    }

    int StageIF::next_cycle() {
        work_h();
        work_l();
        return 0;
    }

    int StageIF::reset() {
        cout << "Stage " << stage_name << " reset\n";

        loaded_instructions.push_back(sr_bank->get(SPECIAL_PC));
        loaded_instruction_index = 1;
        
        return 0;
    }

    uint32_t StageIF::get_loaded_instruction_index() const {
        return loaded_instruction_index;
    }

    std::vector<uint32_t> StageIF::get_loaded_instructions() const {
        return loaded_instructions;
    }

} //namespace