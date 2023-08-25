#ifndef MIPS_SIM_STAGE_WB_H
#define MIPS_SIM_STAGE_WB_H

#include "cpu_stage.h"
#include "i_forwardable_stage.h"
#include "../hardware_manager.h"


namespace mips_sim {
    
    class StageWB: public CpuStage, public IForwardableStage
    {
        
        public:

            StageWB(std::shared_ptr<ControlUnit> control_unit, std::shared_ptr<HardwareManager> hardware_manager);
            
            ~StageWB() = default;

            int work_l() override;

            int work_h() override;

            int next_cycle() override;

            int reset() override;

            // IForwardableStage
            bool forward_register(int regId, int regValue) override;

    };

} //namespace
#endif