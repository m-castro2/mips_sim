#ifndef MIPS_SIM_UTILS_H
#define MIPS_SIM_UTILS_H

#include <cstdint>

namespace mips_sim
{

class Utils
{
public:

  //static uint32_t float_to_word(float f);
  template  <typename T>
  static void float_to_word(T f, uint32_t word[]);

  static float word_to_float(uint32_t f);
  //static double word_to_double(uint32_t f);
};

} /* namespace */
#endif
