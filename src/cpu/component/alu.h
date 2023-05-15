#ifndef MIPS_SIM_ALU_H
#define MIPS_SIM_ALU_H

#include "../../global_defs.h"

namespace mips_sim
{

class Alu
{
  public:
  
    Alu(int mult_delay, int div_delay);
    
    ~Alu();
    
    uint32_t compute_op(uint32_t alu_input_a,
                        uint32_t alu_input_b,
                        uint32_t alu_op) const;

    uint32_t compute_subop(uint32_t alu_input_a,
                           uint32_t alu_input_b,
                           uint8_t shift_amount,
                           uint32_t alu_subop,
                           uint32_t *HI, uint32_t *LO,
                           int *execution_stall);
                               
  private:
    
    int mult_delay;
    int div_delay;
};


} /* namespace */

#endif
