#ifndef MIPS_SIM_HARDWARE_MANAGER_H
#define MIPS_SIM_HARDWARE_MANAGER_H

#include "stages/i_branch_stage.h"

#include <functional>
#include <map>
#include <string_view>

#define SR_INSTRUCTION  0
#define SR_IID          1
#define SR_PC           2
#define SR_SIGNALS      3
#define SR_OPCODE       4
#define SR_RS           5
#define SR_RT           6
#define SR_RD           7
#define SR_SHAMT        8
#define SR_FUNCT        9
#define SR_RSVALUE     10
#define SR_RTVALUE     11
#define SR_ADDR_I      12
#define SR_REGDEST     13
#define SR_ALUOUTPUT   14
#define SR_ALUZERO     15
#define SR_RELBRANCH   16
#define SR_UJUMP       17
#define SR_JUMPADDR    18
#define SR_WORDREAD    19
#define SR_V0          20

#define SIGNAL_PCWRITE "s_pcwrite_enabled"
#define SIGNAL_PCSRC   "s_pcsrc"
#define SIGNAL_CBRANCH "s_cbranch_addr"
#define SIGNAL_RBRANCH "s_rbranch_addr"
#define SIGNAL_JBRANCH "s_jbranch_addr"
#define SIGNAL_FLUSH   "s_pipeline_flush"
#define SIGNAL_FPCOND  "s_fp_conditional_bit"
#define SIGNAL_FPCONDRD "s_fp_conditional_bit_ready"
#define SIGNAL_FP_UNIT_AVAIL "s_unit_avaiable"

namespace mips_sim {
    
    class HardwareManager
    { 
        private:

            int branch_type {};

            int branch_stage {};

            std::map<int, int> status {};

            std::map<std::string_view, std::function<uint32_t( void )>> cpu_signals {};

            std::map<int, uint32_t> stage_instructions {};

            bool fp_stall {};

            int flush_signal {};

            //For GUI program
            std::map<int, std::map<std::string_view, int>> instruction_signal_map {};

            int fp_coprocessor_active_instruction_count;

        public:

            HardwareManager(int branch_type, int branch_stage);
            
            ~HardwareManager();

            int get_branch_type();

            void set_branch_type(int new_type);

            int get_branch_stage();

            void set_branch_stage(int new_stage);

            int get_status(int key);

            bool set_status(int key, int value);

            std::function<uint32_t( void )> get_signal(std::string_view key);

            void set_signal(std::string_view key, std::function<uint32_t( void )>);

            void reset();

            void set_stage_instruction(int stage, uint32_t instruction);

            uint32_t get_stage_instruction(int stage);

            bool get_fp_stall();

            void set_fp_stall(bool value);

            std::map<int, std::map<std::string_view, int>> get_instruction_signal_map();

            void add_instruction_signal(int stage, std::string_view key, int value);

            int get_flush_signal();

            void set_flush_signal(int value);

            int get_fp_coprocessor_active_instructions_count();

            void add_to_fp_coprocessor_active_instructions_count(int value);

    };
} //namespace
#endif