#ifndef MIPS_SIM_FORWARDING_UNIT_H
#define MIPS_SIM_FORWARDING_UNIT_H

#include "control_unit.h"
#include "../../global_defs.h"

#include <cstdint>
#include <memory>

namespace mips_sim
{

  class ForwardingUnit
  {
    private:

        bool enabled =  false;

        std::shared_ptr<ControlUnit> control_unit;

        std::shared_ptr<seg_reg_t> seg_reg_ex_mem {};

        std::shared_ptr<seg_reg_t> seg_reg_mem_wb {};

        std::shared_ptr<seg_reg_t> seg_reg_wb_fwb {};

        int forwarded_from = -1;

    public:
    
        ForwardingUnit(std::shared_ptr<ControlUnit> control_unit, bool enabled);
        ~ForwardingUnit();

        uint32_t forward_register( uint32_t reg, uint32_t reg_value, bool fp_reg, std::ostream &out);

        bool is_enabled();

        void set_enabled(bool value);

        void set_seg_reg_mem_wb(std::shared_ptr<seg_reg_t> seg_reg);

        void set_seg_reg_ex_mem(std::shared_ptr<seg_reg_t> seg_reg);

        void set_seg_reg_wb_fwb(std::shared_ptr<seg_reg_t> seg_reg);

        int get_forwarded_from();

  }; 
} /* namespace */
#endif
