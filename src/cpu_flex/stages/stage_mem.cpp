#include "stage_mem.h"
#include "../hardware_manager.h"

#include <iostream>

using namespace std;

namespace mips_sim {

    StageMEM::StageMEM(std::shared_ptr<Memory> _memory, std::shared_ptr<ControlUnit> control_unit, std::shared_ptr<HardwareManager> hardware_manager,
                    std::initializer_list<signal_t> cpu_signals)
        : memory {_memory}, CpuStage { "MEM", control_unit, hardware_manager, cpu_signals }
    {

    };

    int StageMEM::work_l() {
        cout << "Stage " << stage_name << " work_l\n" ;
        return 0;
    };

    int StageMEM::work_h() {
        cout << "Stage " << stage_name << " work_h\n";
        return 0;
    }

    int StageMEM::next_cycle() {
        work_h();
        work_l();
        return 0;
    }

    int StageMEM::reset() {
        cout << "Stage " << stage_name << " reset\n";
        return 0;
    }

    // IBranchStage
    uint32_t StageMEM::get_sig_pcsrc() const {
        cout << "Stage " << stage_name << " get_sigpcsrc\n";
        return 0;
    }

    uint32_t StageMEM::get_addr_cbranch() const {
        cout << "Stage " << stage_name << " get_addr_cbranch\n";
        return addr_cbranch;
    }

    uint32_t StageMEM::get_addr_rbranch() const {
        cout << "Stage " << stage_name << " get_addr_rbranch\n";
        return addr_rbranch;
    }

    uint32_t StageMEM::get_addr_jbranch() const {
        cout << "Stage " << stage_name << " get_addr_jbranch\n";
        return addr_jbranch;
    }

    // IForwardableStage
    bool StageMEM::forward_register(int regId, int regValue) {
        cout << "Stage " << stage_name << " forward_register\n";
        return 0;
    }


} //namespace