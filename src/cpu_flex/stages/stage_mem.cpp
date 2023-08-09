#include "stage_mem.h"
#include "../hardware_manager.h"

#include <iostream>

using namespace std;

namespace mips_sim {

    StageMEM::StageMEM(std::shared_ptr<Memory> _memory, std::shared_ptr<HardwareManager> hardware_manager)
        : memory {_memory}, CpuStage { "MEM", hardware_manager }
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
    int StageMEM::get_sigpcsrc(int p_sigpcsrc) {
        cout << "Stage " << stage_name << " get_sigpcsrc\n";
        return 0;
    }

    int StageMEM::get_addr_cbranch(int p_addr_cbranch) {
        cout << "Stage " << stage_name << " get_addr_cbranch\n";
        return 0;
    }

    int StageMEM::get_addr_rbranch(int p_addr_rbranch) {
        cout << "Stage " << stage_name << " get_addr_rbranch\n";
        return 0;
    }

    int StageMEM::get_addr_jbranch(int p_addr_jbranch) {
        cout << "Stage " << stage_name << " get_addr_jbranch\n";
        return 0;
    }

    // IForwardableStage
    bool StageMEM::forward_register(int regId, int regValue) {
        cout << "Stage " << stage_name << " forward_register\n";
        return 0;
    }


} //namespace