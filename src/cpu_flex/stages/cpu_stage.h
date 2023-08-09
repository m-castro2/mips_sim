#ifndef MIPS_SIM_CPU_STAGE_H
#define MIPS_SIM_CPU_STAGE_H

#include "../hardware_manager.h"
#include "../../global_defs.h"

#include <memory>
#include <string_view>


namespace mips_sim {
    
    class CpuStage {

        protected:

            const std::string_view stage_name {};

            int sigmask {};

            seg_reg_t seg_reg {};

            seg_reg_t* next_seg_reg {};

            CpuStage* next_stage {};

            int current_instruction {};

            std::shared_ptr<HardwareManager> hardware_manager {};

        
        public:

            CpuStage(std::string_view stage_name, std::shared_ptr<HardwareManager> hardware_manager);
            
            virtual ~CpuStage() = default;

            virtual int work_l() = 0;

            virtual int work_h() = 0;

            virtual int next_cycle() = 0;

            virtual int reset() = 0;

    };

} //namespace
#endif