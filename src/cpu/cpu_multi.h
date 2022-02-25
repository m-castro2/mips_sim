#ifndef MIPS_SIM_CPU_MULTI_H
#define MIPS_SIM_CPU_MULTI_H

#include "cpu.h"

namespace mips_sim
{

class CpuMulti : public Cpu
{
  public:
    CpuMulti(std::shared_ptr<ControlUnit>, std::shared_ptr<Memory>);
    virtual ~CpuMulti() override;

    virtual bool next_cycle( void ) override;

  private:
    uint32_t A_REG;
    uint32_t B_REG;
    uint32_t ALU_OUT_REG;
    uint32_t MEM_DATA_REG;
};

} /* namespace */
#endif
