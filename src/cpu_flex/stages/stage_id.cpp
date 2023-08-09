#include "stage_id.h"
#include "../hardware_manager.h"

#include <iostream>

using namespace std;

namespace mips_sim {

    StageID::StageID(std::shared_ptr<HardwareManager> hardware_manager)
        : CpuStage { "ID", hardware_manager }
    {
        gpr_bank = shared_ptr<GPRegistersBank>(new GPRegistersBank());
        fpr_bank = shared_ptr<FPRegistersBank>(new FPRegistersBank());
        //sr_bank = shared_ptr<SpecialRegistersBank>(new SpecialRegistersBank()); ??
    };

    int StageID::work_l() {
        cout << "Stage " << stage_name << " work_l\n" ;
        return 0;
    };

    int StageID::work_h() {
        cout << "Stage " << stage_name << " work_h\n";
        return 0;
    }

    int StageID::next_cycle() {
        work_h();
        work_l();
        return 0;
    }

    int StageID::reset() {
        cout << "Stage " << stage_name << " reset\n";
        return 0;
    }

    // IBranchStage
    int StageID::get_sigpcsrc(int p_sigpcsrc) {
        cout << "Stage " << stage_name << " get_sigpcsrc\n";
        return 0;
    }

    int StageID::get_addr_cbranch(int p_addr_cbranch) {
        cout << "Stage " << stage_name << " get_addr_cbranch\n";
        return 0;
    }

    int StageID::get_addr_rbranch(int p_addr_rbranch) {
        cout << "Stage " << stage_name << " get_addr_rbranch\n";
        return 0;
    }

    int StageID::get_addr_jbranch(int p_addr_jbranch) {
        cout << "Stage " << stage_name << " get_addr_jbranch\n";
        return 0;
    }

} //namespace