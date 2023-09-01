#ifndef MIPS_SIM_FPCOPROCESSOR_H
#define MIPS_SIM_FPCOPROCESSOR_H

#include "../cpu/component/registers_bank.h"
#include "../../global_defs.h"

#include <memory>



namespace mips_sim
{
#define ADD_UNIT 0
#define MULT_UNIT 1
#define DIV_UNIT 2

struct fp_unit {
    seg_reg_t seg_reg = {};
    int delay {};
    int cycles_elapsed {};
    bool available {};
};

class FPCoprocessor
{
    private:

        bool enabled {};

        //TODO control register for condition bit (p209)

        std::vector<int> delays = {}; //TODO different delays for s and d precision (p233)

        std::vector<int> counts = {};

        std::vector<std::vector<std::shared_ptr<fp_unit>>> fp_units {}; //vector[type][unit]

        std::shared_ptr<FPRegistersBank> fpr_bank {};

    public:
  
        FPCoprocessor(std::vector<int> delays, std::vector<int> counts, std::shared_ptr<FPRegistersBank> fpr_bank);
    
        ~FPCoprocessor();

        bool is_enabled();

        void set_enabled(bool value);

        seg_reg_t work();

        bool is_unit_available(int unit_type);

        void set_seg_reg(int unit_type, seg_reg_t seg_reg);

        void fp_unit_compute(std::shared_ptr<fp_unit> unit);
};

} /* namespace */

#endif
