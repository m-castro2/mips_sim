#include "cpu_stage.h"
#include "../hardware_manager.h"

#include <memory>

namespace mips_sim {

    CpuStage::CpuStage(std::string_view p_stage_name, std::shared_ptr<HardwareManager> p_hardware_manager) 
        : stage_name { p_stage_name }, hardware_manager { p_hardware_manager } {

    };

} //namespace