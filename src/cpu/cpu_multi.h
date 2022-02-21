#ifndef MIPS_SIM_CPU_MULTI_H
#define MIPS_SIM_CPU_MULTI_H

#include "cpu.h"

namespace mips_sim
{

class CpuMulti : public Cpu
{
  public:
    CpuMulti(ControlUnit &, std::shared_ptr<Memory>);
    virtual ~CpuMulti();

    virtual void next_cycle( void );
};

} /* namespace */
#endif
