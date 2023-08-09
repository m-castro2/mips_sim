#ifndef MIPS_SIM_CPU_FLEX_H
#define MIPS_SIM_CPU_FLEX_H

#include "../cpu_pipelined.h"
#include "hardware_manager.h"
#include "stages/cpu_stage.h"

#include <memory>
#include <vector>


namespace mips_sim {
    
    class CpuFlex: public CpuPipelined {

        private:

            std::vector<CpuStage*> cpu_stages {};
            std::shared_ptr<HardwareManager> hardware_manager {};


        public:
            CpuFlex(std::shared_ptr<Memory>,
                std::shared_ptr<ControlUnit> = nullptr,
                int branch_type = BRANCH_NON_TAKEN,
                int branch_stage = STAGE_ID,
                bool has_forwarding_unit = true,
                bool has_hazard_detection_unit = true);
            
            virtual ~CpuFlex() override;

            std::vector<CpuStage*> get_cpu_stages();

            void add_cpu_stage(CpuStage* stage);

            virtual bool next_cycle( std::ostream & = std::cout ) override;

    };
} //namespace
#endif