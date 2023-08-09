#ifndef MIPS_SIM_STAGE_EX_H
#define MIPS_SIM_STAGE_EX_H

#include "cpu_stage.h"
#include "../hardware_manager.h"
#include "../../cpu/component/alu.h"

#include <memory>


namespace mips_sim {
    
    class StageEX: public CpuStage
    {
        private:
            
            std::unique_ptr<Alu> alu {};
        
        public:

            StageEX(int mult_delay, int div_delay, std::shared_ptr<HardwareManager> hardware_manager);
            
            ~StageEX() = default;

            int work_l() override;

            int work_h() override;

            int next_cycle() override;

            int reset() override;

    };

} //namespace
#endif