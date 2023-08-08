#include "stage_wb.h"

#include <iostream>

using namespace std;

namespace mips_sim {

    StageWB::StageWB() : CpuStage { "WB" } {

    };

    int StageWB::work_l() {
        cout << "Stage " << stage_name << " work_l\n" ;
        return 0;
    };

    int StageWB::work_h() {
        cout << "Stage " << stage_name << " work_h\n";
        return 0;
    }

    int StageWB::next_cycle() {
        work_h();
        work_l();
        return 0;
    }

    int StageWB::reset() {
        cout << "Stage " << stage_name << " reset\n";
        return 0;
    }

    // IForwardableStage
    bool StageWB::forward_register(int regId, int regValue) {
        cout << "Stage " << stage_name << " forward_register\n";
        return 0;
    }


} //namespace