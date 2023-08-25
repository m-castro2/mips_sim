#ifndef MIPS_SIM_STAGE_EX_H
#define MIPS_SIM_STAGE_EX_H

#include "cpu_stage.h"
#include "../hardware_manager.h"
#include "../../cpu/component/alu.h"
#include "../../cpu/component/control_unit.h"
#include "../../cpu/component/forwarding_unit.h"
#include "../../cpu/component/registers_bank.h"

#include <memory>


namespace mips_sim {
    
    class StageEX: public CpuStage
    {
        private:
            
            std::unique_ptr<Alu> alu {};

            std::shared_ptr<SpecialRegistersBank> sr_bank {};

            uint32_t instruction_code {};
            uint32_t microinstruction {};
            uint32_t pc_value {};
            uint32_t addr_i32 {};
            uint32_t alu_output {};
            uint32_t opcode {};
            uint32_t rt_value {};
            uint32_t hi_reg {};
            uint32_t lo_reg {};
            int stall_cycles {};
            bool hi_lo_updated {};
        
        public:

            StageEX(int mult_delay, int div_delay, std::shared_ptr<ControlUnit> control_unit, std::shared_ptr<HardwareManager> hardware_manager,
                    std::initializer_list<signal_t> cpu_signals, std::shared_ptr<ForwardingUnit> fu, std::shared_ptr<SpecialRegistersBank> sr_bank);
            
            ~StageEX() = default;

            int work_l() override;

            int work_h() override;

            int next_cycle() override;

            int reset() override;

    };

} //namespace
#endif