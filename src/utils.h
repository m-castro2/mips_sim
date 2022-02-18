#ifndef MIPS_SIM_UTILS_H
#define MIPS_SIM_UTILS_H

#include <cstdint>

namespace mips_sim
{

class Utils
{
public:
  static uint32_t float_to_word(float f);
};

} /* namespace */
#endif
