#include "hazard_detection_unit.h"

#include <iostream>
#include <cassert>

using namespace std;

namespace mips_sim
{

    HazardDetectionUnit::HazardDetectionUnit(std::shared_ptr<ControlUnit> p_control_unit, bool p_enabled) {
        control_unit = p_control_unit;
        enabled = p_enabled;
    }
    HazardDetectionUnit::~HazardDetectionUnit() {}

    void HazardDetectionUnit::set_enabled(bool value) {
        enabled = value;
    }

    bool HazardDetectionUnit::is_enabled() {
        return enabled;
    }

    uint32_t HazardDetectionUnit::detect_hazard( uint32_t read_reg, bool can_forward, bool fp_reg, bool fpu) const
    {    
        if(!enabled)
        {
            //TODO: Check for hazards anyway and keep the info.
            //TODO: Inform about ignored hazard
            return false;
        }
        
        bool hazard;

        /* check next 2 registers as forwarding would happen on next cycle */
        uint32_t ex_regdest   = seg_reg_id_ex->data[SR_REGDEST];
        uint32_t ex_regwrite  = control_unit->test(seg_reg_id_ex->data[SR_SIGNALS], SIG_REGWRITE);
        uint32_t ex_fpwrite   = control_unit->test(seg_reg_id_ex->data[SR_SIGNALS], SIG_REGBANK);
        uint32_t ex_memread   = control_unit->test(seg_reg_id_ex->data[SR_SIGNALS], SIG_MEMREAD);
        uint32_t mem_regdest  = seg_reg_ex_mem->data[SR_REGDEST];
        uint32_t mem_regwrite = control_unit->test(seg_reg_ex_mem->data[SR_SIGNALS], SIG_REGWRITE);
        uint32_t mem_fpwrite  = control_unit->test(seg_reg_ex_mem->data[SR_SIGNALS], SIG_REGBANK);

        bool ex_regtype_match = !(ex_fpwrite ^ fp_reg);
        bool mem_regtype_match = !(mem_fpwrite ^ fp_reg);

        hazard =     (ex_regdest == read_reg)
                && ex_regwrite
                && ex_regtype_match
                && (ex_memread || !can_forward);
        hazard |=    (mem_regdest == read_reg)
                && mem_regtype_match
                && mem_regwrite
                && !can_forward;
        
        if (!fpu) {
            return hazard;
        }
        
        for (auto dest_reg: *dest_registers) {
            bool finished = (fpu_forwarding_registers->find(dest_reg) != fpu_forwarding_registers->end()); // FPU instruction is finished
            hazard |= (read_reg == dest_reg && can_forward && !finished);
        }

        return hazard;


    }

    void HazardDetectionUnit::set_seg_reg_id_ex(std::shared_ptr<seg_reg_t> seg_reg) {
        seg_reg_id_ex = seg_reg;
    }

    void HazardDetectionUnit::set_seg_reg_ex_mem(std::shared_ptr<seg_reg_t> seg_reg) {
        seg_reg_ex_mem = seg_reg;
    }

    void HazardDetectionUnit::set_fpu_dest_registers(std::shared_ptr<std::vector<uint32_t>> p_dest_registers) {
        dest_registers = p_dest_registers;
    }

    void HazardDetectionUnit::set_fpu_forwarding_registers(std::shared_ptr<std::map<uint32_t, uint32_t>> forwarding_registers) {
        fpu_forwarding_registers = forwarding_registers;
    }
  
} /* namespace */
