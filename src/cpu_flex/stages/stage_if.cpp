#include "stage_if.h"
#include "../hardware_manager.h"

#include <iostream>

using namespace std;

namespace mips_sim {

    StageIF::StageIF(std::shared_ptr<Memory> _memory, std::shared_ptr<HardwareManager> hardware_manager)
        : memory {_memory}, CpuStage { "IF" , hardware_manager }
    {

    };

    int StageIF::work_l() {
        cout << "Stage " << stage_name << " work_l\n" ;
        return 0;
    };

    int StageIF::work_h() {
        cout << "Stage " << stage_name << " work_h\n";
        return 0;
    }

    int StageIF::next_cycle() {
        work_h();
        work_l();
        return 0;
    }

    int StageIF::reset() {
        cout << "Stage " << stage_name << " reset\n";
        return 0;
    }

} //namespace