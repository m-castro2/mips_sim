#ifndef MIPS_SIM_CPU_STAGE_H
#define MIPS_SIM_CPU_STAGE_H


#include <string_view>


namespace mips_sim {
    
    class CpuStage {

        protected:

            const std::string_view stage_name {};

            int sigmask {};

            int seg_reg {};

            int* next_seg_reg {}; //next_stage->seg_reg??

            CpuStage* next_stage {};

            int current_instruction {};

        
        public:

            CpuStage(std::string_view stage_name);
            
            virtual ~CpuStage() = default;

            virtual int work_l() = 0;

            virtual int work_h() = 0;

            virtual int next_cycle() = 0;

            virtual int reset() = 0;

    };

} //namespace
#endif