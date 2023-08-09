#ifndef MIPS_SIM_STAGE_IF_H
#define MIPS_SIM_STAGE_IF_H

#include "cpu_stage.h"
#include "../hardware_manager.h"
#include "../../mem.h"

#include <memory>

namespace mips_sim {
    
    class StageIF: public CpuStage
    {   
        private:

            std::shared_ptr<Memory> memory {};

        public:

            StageIF(std::shared_ptr<Memory> memory, std::shared_ptr<HardwareManager> hardware_manager);
            
            ~StageIF() = default;

            int work_l() override;

            int work_h() override;

            int next_cycle() override;

            int reset() override;

    };

} //namespace
#endif