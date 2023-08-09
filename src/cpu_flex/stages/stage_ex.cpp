#include "stage_ex.h"
#include "../hardware_manager.h"

#include <iostream>

using namespace std;

namespace mips_sim {

    StageEX::StageEX(int mult_delay, int div_delay, std::shared_ptr<HardwareManager> hardware_manager)
        : CpuStage { "EX", hardware_manager } 
    {
        alu = unique_ptr<Alu>(new Alu(mult_delay, div_delay));
    };

    int StageEX::work_l() {
        cout << "Stage " << stage_name << " work_l\n" ;
        return 0;
    };

    int StageEX::work_h() {
        cout << "Stage " << stage_name << " work_h\n";
        return 0;
    }

    int StageEX::next_cycle() {
        work_h();
        work_l();
        return 0;
    }

    int StageEX::reset() {
        cout << "Stage " << stage_name << " reset\n";
        return 0;
    }

} //namespace