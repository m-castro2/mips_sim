#ifndef MIPS_SIM_HAZARD_DETECTION_UNIT_H
#define MIPS_SIM_HAZARD_DETECTION_UNIT_H

#include "../../global_defs.h"
#include "control_unit.h"

#include <cstdint>
#include <map>
#include <memory>

namespace mips_sim
{

  class HazardDetectionUnit
  {
    private:
        bool enabled = {};

        std::shared_ptr<ControlUnit> control_unit;

        std::shared_ptr<seg_reg_t> seg_reg_id_ex;

        std::shared_ptr<seg_reg_t> seg_reg_ex_mem;

        std::shared_ptr<std::vector<uint32_t>> dest_registers {};

        std::shared_ptr<std::map<uint32_t, uint32_t>> fpu_forwarding_registers {};

    public:
    
        HazardDetectionUnit(std::shared_ptr<ControlUnit> control_unit, bool enabled);
        ~HazardDetectionUnit();

        uint32_t detect_hazard(uint32_t read_reg, bool can_forward, bool fp_reg = false, bool fpu = false) const;
        
        bool is_enabled();

        void set_enabled(bool value);

        void set_seg_reg_id_ex(std::shared_ptr<seg_reg_t> seg_reg);

        void set_seg_reg_ex_mem(std::shared_ptr<seg_reg_t> seg_reg);

        void set_fpu_dest_registers(std::shared_ptr<std::vector<uint32_t>> dest_registers);

        void set_fpu_forwarding_registers(std::shared_ptr<std::map<uint32_t, uint32_t>> forwarding_registers);

  };
} /* namespace */
#endif
