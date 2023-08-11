#ifndef MIPS_SIM_STAGE_MEM_H
#define MIPS_SIM_STAGE_MEM_H

#include "cpu_stage.h"
#include "i_branch_stage.h"
#include "i_forwardable_stage.h"
#include "../hardware_manager.h"
#include "../../mem.h"

#include <memory>

namespace mips_sim {
    
    class StageMEM: public CpuStage, public IBranchStage, public IForwardableStage
    {   
        private:

            std::shared_ptr<Memory> memory {};
        
        public:

            StageMEM(std::shared_ptr<Memory> memory, std::shared_ptr<HardwareManager> hardware_manager);
            
            ~StageMEM() = default;

            int work_l() override;

            int work_h() override;

            int next_cycle() override;

            int reset() override;

            // IBranchStage
            uint32_t get_sig_pcsrc() override;

            int get_addr_cbranch(int p_addr_cbranch) override;

            int get_addr_rbranch(int p_addr_rbranch) override;

            int get_addr_jbranch(int p_addr_jbranch) override;

            // IForwardableStage
            bool forward_register(int regId, int regValue) override;

    };

} //namespace
#endif