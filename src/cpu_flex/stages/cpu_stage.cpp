#include "cpu_stage.h"
#include "../hardware_manager.h"
#include "../../cpu/component/control_unit.h"
#include "../../cpu/component/hazard_detection_unit.h"
#include "../../cpu/component/forwarding_unit.h"

#include <memory>

namespace mips_sim {

    CpuStage::CpuStage(std::string_view p_stage_name, std::shared_ptr<ControlUnit> p_control_unit,
               std::shared_ptr<HardwareManager> p_hardware_manager,
               std::initializer_list<signal_t> p_cpu_signals,
               std::shared_ptr<HazardDetectionUnit> p_hdu,
               std::shared_ptr<ForwardingUnit> p_fu)
        : stage_name { p_stage_name }, hardware_manager { p_hardware_manager }, 
        hdu { p_hdu }, fu { p_fu }, control_unit { p_control_unit }
    {   
        this->seg_reg = std::shared_ptr<seg_reg_t>(new seg_reg_t{});
        cpu_signals = p_cpu_signals;
        seg_reg_wrflag = false;
    };

    void CpuStage::set_sigmask(uint32_t p_sigmask)  {
        sigmask = p_sigmask;
    }

    std::shared_ptr<seg_reg_t> CpuStage::get_seg_reg() {
        return seg_reg;
    }

    void CpuStage::set_seg_reg(seg_reg_t next_seg_reg) {
        for (int i=0; i < 32; ++i) {
            seg_reg->data[i] = next_seg_reg.data[i];
        }
    }

    seg_reg_t CpuStage::get_next_seg_reg() {
        return next_seg_reg;
    }

    bool CpuStage::write_segmentation_register(seg_reg_t p_tmp_seg_reg) {
        if (seg_reg_wrflag) {
            //already written this cycle
            return false;
        }

        next_seg_reg = p_tmp_seg_reg;
        seg_reg_wrflag = true;
        
        return true;
    }

    error_exception_t CpuStage::get_error(){
        return error;
    }

} //namespace