#ifndef MIPS_SIM_BRANCH_STAGE_H
#define MIPS_SIM_BRANCH_STAGE_H

#include <cstdint>

namespace mips_sim {
    
    class IBranchStage {

        protected:

            uint32_t sig_pcsrc {};

            uint32_t addr_cbranch {};

            uint32_t addr_rbranch {};

            uint32_t addr_jbranch {};

            uint32_t pipeline_flush_signal;
        
        public:
            
            virtual ~IBranchStage() = default;

            virtual uint32_t get_sig_pcsrc() const = 0;

            virtual uint32_t get_addr_cbranch() const = 0;

            virtual uint32_t get_addr_rbranch() const = 0;

            virtual uint32_t get_addr_jbranch() const = 0;

            virtual uint32_t get_pipeline_flush_signal() const = 0;

            virtual void status_update() = 0;

    };

} //namespace
#endif
