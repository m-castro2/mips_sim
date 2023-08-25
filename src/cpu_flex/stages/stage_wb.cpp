#include "stage_wb.h"
#include "../hardware_manager.h"

#include <iostream>

using namespace std;

namespace mips_sim {

    StageWB::StageWB(std::shared_ptr<ControlUnit> control_unit, std::shared_ptr<HardwareManager> hardware_manager)
        : CpuStage { "WB", control_unit, hardware_manager }
    {

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