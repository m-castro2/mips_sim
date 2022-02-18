#include "utils.h"
#include <cmath>
#include <cstdio>

namespace mips_sim
{

uint32_t Utils::float_to_word(float f)
{
  uint32_t v = 0;

  uint32_t tm = 23, te = 8, excess = 127;
  uint32_t sign;
  uint32_t exp;
  uint32_t mantissa;

  sign = static_cast<uint32_t>((f < 0) << 31);
  f = fabsf(f);

  uint32_t integer_part = static_cast<uint32_t>(floorf(f));
  float decimal_part = f - static_cast<float>(integer_part);

  /* convert decimal */
  uint32_t decpart = 0;
  for (size_t i=0; i<tm; ++i)
  {
    decimal_part *= 2;
    if (decimal_part > 1)
    {
      decpart |= 1;
      decimal_part -= 1.0f;
    }
    decpart <<= 1;
  }
  decpart <<= 31 - tm;

  mantissa = decpart;
  exp = excess;
  if (integer_part > 1)
  {
    while (integer_part > 1)
    {
       mantissa >>= 1;
       mantissa &= 0x7fffffff;
       mantissa |= ((integer_part << 31) & 0x80000000);
       integer_part >>= 1;
       exp ++;
    }
  }

  v = sign;
  v |= ((exp & ((1<<te)-1)) << (31-te));
  v |= ((mantissa >> (32-tm)) & ((1<<tm)-1));

  return v;
}

} /* namespace */
