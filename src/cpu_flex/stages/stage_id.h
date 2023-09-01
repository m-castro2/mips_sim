#ifndef MIPS_SIM_STAGE_ID_H
#define MIPS_SIM_STAGE_ID_H

#include "cpu_stage.h"
#include "i_branch_stage.h"
#include "../hardware_manager.h"
#include "../../cpu/component/registers_bank.h"
#include "../../cpu/component/control_unit.h"
#include "../../cpu/component/hazard_detection_unit.h"
#include "../../cpu/component/forwarding_unit.h"

#include <memory>


namespace mips_sim {
    
    class StageID: public CpuStage, public IBranchStage
    {
        private:

            std::shared_ptr<GPRegistersBank> gpr_bank {}; /* general purpose registers */
            std::shared_ptr<FPRegistersBank> fpr_bank {}; /* floating point registers */
            std::shared_ptr<SpecialRegistersBank> sr_bank {};

            uint32_t read_register(uint8_t reg_index);
            uint32_t read_fp_register(uint8_t reg_index);

            bool pc_write = 1;

            //coprocessor
            int fp_unit_type; //FPUnit type
        
        public:

            StageID(std::shared_ptr<ControlUnit> control_unit, std::shared_ptr<HardwareManager> hardware_manager,
                    std::initializer_list<signal_t> cpu_signals, std::shared_ptr<HazardDetectionUnit> hdu, std::shared_ptr<ForwardingUnit> fu,
                    std::shared_ptr<GPRegistersBank> gpr_bank, std::shared_ptr<FPRegistersBank> fpr_bank, 
                    std::shared_ptr<SpecialRegistersBank> sr_bank);
            
            ~StageID() = default;

            int work_l() override;

            int work_h() override;

            int rising_flank() override;

            int next_cycle() override;

            int reset() override;

            bool process_branch(instruction_t instruction, uint32_t rs_value, uint32_t rt_value, uint32_t pc_value);

            void status_update();

            bool get_pc_write();

            int send_to_cp1();

            // IBranchStage
            uint32_t get_sig_pcsrc() const override;

            uint32_t get_addr_cbranch() const override;

            uint32_t get_addr_rbranch() const override;

            uint32_t get_addr_jbranch() const override;

    };

} //namespace
#endif