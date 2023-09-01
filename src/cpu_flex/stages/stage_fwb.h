#ifndef MIPS_SIM_STAGE_FWB_H
#define MIPS_SIM_STAGE_FWB_H

#include "cpu_stage.h"
#include "i_forwardable_stage.h"
#include "../hardware_manager.h"
#include "../../cpu/component/registers_bank.h"


namespace mips_sim {
    
    class StageFWB: public CpuStage, public IForwardableStage
    {   
        private:
            std::shared_ptr<GPRegistersBank> gpr_bank {}; /* general purpose registers */
            std::shared_ptr<FPRegistersBank> fpr_bank {}; /* floating point registers */
        
        public:

            StageFWB(std::shared_ptr<ControlUnit> control_unit, std::shared_ptr<HardwareManager> hardware_manager, 
                    std::shared_ptr<GPRegistersBank> gpr_bank, std::shared_ptr<FPRegistersBank> fpr_bank);
            
            ~StageFWB() = default;

            int work_l() override;

            int work_h() override;

            int rising_flank() override;

            int next_cycle() override;

            int reset() override;

            void write_register( uint8_t reg_index, uint32_t value);

            void write_fp_register( uint8_t reg_index, uint32_t value);

            // IForwardableStage
            bool forward_register(int regId, int regValue) override;
    };

} //namespace
#endif