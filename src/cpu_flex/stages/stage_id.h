#ifndef MIPS_SIM_STAGE_ID_H
#define MIPS_SIM_STAGE_ID_H

#include "cpu_stage.h"
#include "i_branch_stage.h"
#include "../../cpu/component/registers_bank.h"

#include <memory>


namespace mips_sim {
    
    class StageID: public CpuStage, public IBranchStage
    {
        private:

            std::shared_ptr<GPRegistersBank> gpr_bank {}; /* general purpose registers */
            std::shared_ptr<FPRegistersBank> fpr_bank {}; /* floating point registers */
            //std::shared_ptr<SpecialRegistersBank> sr_bank;  /* special registers */ add?
        
        public:

            StageID();
            
            ~StageID() = default;

            int work_l() override;

            int work_h() override;

            int next_cycle() override;

            int reset() override;

            // IBranchStage
            int get_sigpcsrc(int p_sigpcsrc) override;

            int get_addr_cbranch(int p_addr_cbranch) override;

            int get_addr_rbranch(int p_addr_rbranch) override;

            int get_addr_jbranch(int p_addr_jbranch) override;

    };

} //namespace
#endif