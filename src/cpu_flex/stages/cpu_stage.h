#ifndef MIPS_SIM_CPU_STAGE_H
#define MIPS_SIM_CPU_STAGE_H

#include "../hardware_manager.h"
#include "../../global_defs.h"
#include "../../cpu/component/control_unit.h"
#include "../../cpu/component/hazard_detection_unit.h"
#include "../../cpu/component/forwarding_unit.h"

#include <memory>
#include <string_view>


namespace mips_sim {
    
    class CpuStage {

        protected:

            const std::string_view stage_name {};

            uint32_t sigmask {};

            std::shared_ptr<seg_reg_t>  seg_reg {};

            seg_reg_t next_seg_reg {};

            seg_reg_t tmp_seg_reg {};

            bool seg_reg_wrflag {};

            CpuStage* next_stage {};

            int current_instruction {};

            std::shared_ptr<ControlUnit> control_unit {};

            std::shared_ptr<HardwareManager> hardware_manager {};

            std::initializer_list<signal_t> cpu_signals {};

            std::shared_ptr<HazardDetectionUnit> hdu {};

            std::shared_ptr<ForwardingUnit> fu {};

        
        public:

            CpuStage(std::string_view stage_name, std::shared_ptr<ControlUnit> control_unit,
               std::shared_ptr<HardwareManager> hardware_manager,
               std::initializer_list<signal_t> cpu_signals = {},
               std::shared_ptr<HazardDetectionUnit> hdu = nullptr,
               std::shared_ptr<ForwardingUnit> fu = nullptr);
            
            virtual ~CpuStage() = default;

            virtual int work_l() = 0;

            virtual int work_h() = 0;

            virtual int rising_flank() = 0;

            virtual int next_cycle() = 0;

            virtual int reset() = 0;

            std::shared_ptr<seg_reg_t> get_seg_reg();

            void set_seg_reg(seg_reg_t next_seg_reg);

            seg_reg_t get_next_seg_reg();

            void set_sigmask(uint32_t p_sigmask);

            bool write_segmentation_register(seg_reg_t value);

    };

} //namespace
#endif