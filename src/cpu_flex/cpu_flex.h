#ifndef MIPS_SIM_CPU_FLEX_H
#define MIPS_SIM_CPU_FLEX_H

#include "../cpu_pipelined.h"
#include "hardware_manager.h"
#include "stages/cpu_stage.h"
#include "../cpu/component/forwarding_unit.h"
#include "../cpu/component/hazard_detection_unit.h"
#include "../../cpu/component/registers_bank.h"
#include "fp_coprocessor.h"

#include <memory>
#include <vector>


namespace mips_sim {
    
    class CpuFlex: public CpuPipelined {

        private:

            std::vector<CpuStage*> cpu_stages {};

            std::shared_ptr<HardwareManager> hardware_manager {};

            std::shared_ptr<HazardDetectionUnit> hdu {};

            std::shared_ptr<ForwardingUnit> fu {};

            std::shared_ptr<FPCoprocessor> cp1 {};


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

            virtual void reset( bool reset_data_memory = true,
                        bool reset_text_memory = true ) override;

            size_t get_current_instruction(size_t stage) const;

            virtual std::vector<uint32_t> get_loaded_instructions() const;

            bool run_to_cycle( uint32_t target_cycle, std::ostream & = std::cout);

            void print_diagram( std::ostream &out) const;

            std::vector<std::vector<int>> get_stage_instruction_signals(int stage_count);

            std::map<int, std::map<std::string_view, int>> get_hw_stage_instruction_signals(int stage_count);

    };
} //namespace
#endif