#ifndef MIPS_SIM_STAGE_IF_H
#define MIPS_SIM_STAGE_IF_H

#include "cpu_stage.h"
#include "../hardware_manager.h"
#include "../../mem.h"
#include "../../cpu/component/control_unit.h"
#include "../../cpu/component/registers_bank.h"

#include <memory>

namespace mips_sim {
    
    class StageIF: public CpuStage
    {   
        private:

            std::shared_ptr<Memory> memory {};

            std::shared_ptr<SpecialRegistersBank> sr_bank {};

            uint32_t loaded_instruction_index {};

            std::vector<uint32_t> loaded_instructions;

            uint32_t delayed_pc {};

            bool delay = false;

        public:

            StageIF(std::shared_ptr<Memory> memory, std::shared_ptr<HardwareManager> hardware_manager, 
                    std::shared_ptr<ControlUnit> control_unit, std::shared_ptr<SpecialRegistersBank> sr_bank);
            
            ~StageIF() = default;

            int work_l() override;

            int work_h() override;

            int rising_flank() override;

            int next_cycle() override;

            int reset() override;

            uint32_t get_loaded_instruction_index() const;

            std::vector<uint32_t> get_loaded_instructions() const;



    };

} //namespace
#endif