#ifndef MIPS_SIM_STAGE_WB_H
#define MIPS_SIM_STAGE_WB_H

#include "cpu_stage.h"
#include "i_forwardable_stage.h"


namespace mips_sim {
    
    class StageWB: public CpuStage, public IForwardableStage
    {
        
        public:

            StageWB();
            
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