#include "forwarding_unit.h"
#include "../../utils.h"

#include <iostream>
#include <cassert>

using namespace std;

namespace mips_sim
{

    ForwardingUnit::ForwardingUnit(std::shared_ptr<ControlUnit> p_control_unit, bool p_enabled)
        : control_unit { p_control_unit }, enabled { p_enabled } 
    {

    }

    ForwardingUnit::~ForwardingUnit() {}

    void ForwardingUnit::set_enabled(bool value) {
        enabled = value;
    }

    bool ForwardingUnit::is_enabled() {
        return enabled;
    }

    uint32_t ForwardingUnit::forward_register( uint32_t reg, uint32_t reg_value, bool fp_reg, std::ostream &out) const 
    {    
        if (!enabled)
            return UNDEF32;

        uint32_t mem_regdest  = seg_reg_ex_mem->data[SR_REGDEST];
        uint32_t mem_regvalue = seg_reg_ex_mem->data[SR_ALUOUTPUT];
        uint32_t mem_fpwrite  = control_unit->test(seg_reg_ex_mem->data[SR_SIGNALS], SIG_REGBANK);
        uint32_t wb_regdest   = seg_reg_mem_wb->data[SR_REGDEST];
        uint32_t wb_regvalue; /* can come from ALU output or Memory */
        uint32_t wb_fpwrite  = control_unit->test(seg_reg_mem_wb->data[SR_SIGNALS], SIG_REGBANK);
        uint32_t fwb_regdest = seg_reg_wb_fwb->data[SR_REGDEST];
        uint32_t fwb_regvalue; /* can come from ALU output or Memory */
        uint32_t fwb_fpwrite  = control_unit->test(seg_reg_wb_fwb->data[SR_SIGNALS], SIG_REGBANK);

        if (reg == 0 && !fp_reg)
            return reg_value;

        /* check EX/MEM register */
        if  (mem_regdest == reg
            && !(mem_fpwrite ^ fp_reg)
            && !control_unit->test(seg_reg_ex_mem->data[SR_SIGNALS], SIG_MEMREAD)
            && control_unit->test(seg_reg_ex_mem->data[SR_SIGNALS], SIG_REGWRITE))
        {
        out << " -- forward "
            << (fp_reg?Utils::get_fp_register_name(reg)
                    :Utils::get_register_name(reg))
            << " [0x" << Utils::hex32(mem_regvalue) << "] from EX/MEM" << endl;
        return mem_regvalue;
        }

        /* check MEM/WB register */
        if (wb_regdest == reg
            && !(wb_fpwrite ^ fp_reg)
            && control_unit->test(seg_reg_mem_wb->data[SR_SIGNALS], SIG_REGWRITE))
        {
        switch (control_unit->test(seg_reg_mem_wb->data[SR_SIGNALS], SIG_MEM2REG))
        {
            case 0:
            wb_regvalue = seg_reg_mem_wb->data[SR_WORDREAD];
            break;
            case 1:
            wb_regvalue = seg_reg_mem_wb->data[SR_ALUOUTPUT];
            break;
            case 2:
            wb_regvalue = seg_reg_mem_wb->data[SR_PC];
            break;
            default:
            assert(0);
        }
        out << " -- forward "
            << (fp_reg?Utils::get_fp_register_name(reg)
                        :Utils::get_register_name(reg))
            << " [0x" << Utils::hex32(wb_regvalue) << "] from MEM/WB" << endl;
        return wb_regvalue;
        }

        /* check WB/FWB register */
        if (fwb_regdest == reg
            && !(fwb_fpwrite ^ fp_reg)
            && control_unit->test(seg_reg_wb_fwb->data[SR_SIGNALS], SIG_REGWRITE))
        {
        switch (control_unit->test(seg_reg_wb_fwb->data[SR_SIGNALS], SIG_MEM2REG))
        {
            case 0:
            fwb_regvalue = seg_reg_wb_fwb->data[SR_WORDREAD];
            break;
            case 1:
            fwb_regvalue = seg_reg_wb_fwb->data[SR_ALUOUTPUT];
            break;
            case 2:
            fwb_regvalue = seg_reg_wb_fwb->data[SR_PC];
            break;
            default:
            assert(0);
        }
        out << " -- forward "
            << (fp_reg?Utils::get_fp_register_name(reg)
                        :Utils::get_register_name(reg))
            << " [0x" << Utils::hex32(fwb_regvalue) << "] from WB/FWB" << endl;
        return fwb_regvalue;
        }

        return reg_value;
    }

    void ForwardingUnit::set_seg_reg_mem_wb(std::shared_ptr<seg_reg_t> seg_reg) {
        seg_reg_mem_wb = seg_reg;
    }

    void ForwardingUnit::set_seg_reg_ex_mem(std::shared_ptr<seg_reg_t> seg_reg) {
        seg_reg_ex_mem = seg_reg;
    }

    void ForwardingUnit::set_seg_reg_wb_fwb(std::shared_ptr<seg_reg_t> seg_reg){
        seg_reg_wb_fwb = seg_reg;
    }
  
} /* namespace */
