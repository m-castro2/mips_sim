#ifndef MIPS_SIM_STAGE_MEM_H
#define MIPS_SIM_STAGE_MEM_H

#include "cpu_stage.h"
#include "i_branch_stage.h"
#include "i_forwardable_stage.h"
#include "../hardware_manager.h"
#include "../../cpu/component/control_unit.h"
#include "../../cpu/component/registers_bank.h"
#include "../../mem.h"

#include <cassert>
#include <memory>

namespace mips_sim {
    
    class StageMEM: public CpuStage, public IBranchStage, public IForwardableStage
    {   
        private:

            std::shared_ptr<Memory> memory {};

            std::shared_ptr<SpecialRegistersBank> sr_bank {};
        
        public:

            StageMEM(std::shared_ptr<Memory> memory, std::shared_ptr<ControlUnit> control_unit, std::shared_ptr<HardwareManager> hardware_manager,
                    std::initializer_list<signal_t> cpu_signals, std::shared_ptr<SpecialRegistersBank> sr_bank);
            
            ~StageMEM() = default;

            int work_l() override;

            int work_h() override;

            int rising_flank() override;

            int next_cycle() override;

            int reset() override;

            // IBranchStage
            uint32_t get_sig_pcsrc() const override;

            uint32_t get_addr_cbranch() const override;

            uint32_t get_addr_rbranch() const override;

            uint32_t get_addr_jbranch() const override;

            // IForwardableStage
            bool forward_register(int regId, int regValue) override;

    };

} //namespace
#endif