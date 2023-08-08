#ifndef MIPS_SIM_BRANCH_STAGE_H
#define MIPS_SIM_BRANCH_STAGE_H


namespace mips_sim {
    
    class IBranchStage {

        protected:

            int sigpcsrc {};

            int addr_cbranch {};

            int addr_rbranch {};

            int addr_jbranch {};

            int pipeline_flush_stages();
        
        public:
            
            virtual ~IBranchStage() = default;

            virtual int get_sigpcsrc(int p_sigpcsrc) = 0;

            virtual int get_addr_cbranch(int p_addr_cbranch) = 0;

            virtual int get_addr_rbranch(int p_addr_rbranch) = 0;

            virtual int get_addr_jbranch(int p_addr_jbranch) = 0;

    };

} //namespace
#endif