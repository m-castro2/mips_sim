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
            std::shared_ptr<GPRegistersBank> gpr_bank {};

            std::function<syscall_struct_t( uint32_t )> syscall;

            syscall_struct_t syscall_struct {};
        
        public:

            StageEX(int mult_delay, int div_delay, std::shared_ptr<ControlUnit> control_unit, std::shared_ptr<HardwareManager> hardware_manager,
                    std::initializer_list<signal_t> cpu_signals, std::shared_ptr<ForwardingUnit> fu, std::shared_ptr<SpecialRegistersBank> sr_bank,
                    std::shared_ptr<GPRegistersBank> gpr_bank);
            
            ~StageEX() = default;

            int work_l() override;

            int work_h() override;

            int rising_flank() override;

            int next_cycle() override;

            int reset() override;

            void set_syscall(std::function<syscall_struct_t( uint32_t )>);

            syscall_struct_t get_syscall_struct();

    };

} //namespace
#endif