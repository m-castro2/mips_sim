#include "utils.h"
#include <cmath>
#include <cstdio>
#include <cassert>
#include <iostream>

namespace mips_sim
{

  static void _float_to_word(double f,
                             uint32_t te, uint32_t tm,
                             uint32_t word[])
  {
    uint64_t v = 0;
    uint32_t excess = (1<<(te-1))-1;

    uint32_t bitcount = te + tm + 1;
    assert(bitcount == 32 || bitcount == 64);

    uint64_t sign;
    uint64_t exp;
    uint64_t mantissa;

    uint64_t integer_part, decpart;
    double decimal_part;

    sign = f < 0;
    sign <<= (bitcount-1);
    f = fabs(f);

    integer_part = static_cast<uint64_t>(floor(f));
    decimal_part = f - static_cast<double>(integer_part);

    /* convert decimal */
    decpart = 0;
    for (size_t i=0; i<tm; ++i)
    {
      decimal_part *= 2;
      if (decimal_part >= 1)
      {
        decpart |= 1;
        decimal_part -= 1.0f;
      }
      decpart <<= 1LL;
    }

    decpart <<= bitcount - tm - 1;
    mantissa = decpart;

    exp = excess;
    if (integer_part > 1)
    {
      while (integer_part > 1)
      {
         mantissa >>= 1;
         mantissa &= (1ULL << (bitcount-1))-1; //0x7fffffffffffffff;
         mantissa |= ((integer_part << (bitcount-1)) & (1ULL<<(bitcount-1)));//0x8000000000000000);
         integer_part >>= 1;
         exp ++;
      }
    }
    else if (integer_part < 1)
    {
      printf("Not implemented IEEE754 for values < 1\n");
      exit(1);
    }

    v = sign;
    v |= ((exp & ((11ULL<<te)-1)) << (bitcount-te-1));
    v |= ((mantissa >> (bitcount-tm)) & ((1ULL<<tm)-1));

    if (bitcount == 64)
    {
      word[1] = static_cast<uint32_t>(v & 0xFFFFFFFF);
      word[0] = static_cast<uint32_t>((v & 0xFFFFFFFF00000000)>>32);
    }
    else
    {
      word[0] = static_cast<uint32_t>(v & 0xFFFFFFFFLL);
    }
  }

  template  <>
  void Utils::float_to_word(double f, uint32_t word[])
  {
    _float_to_word(f, 11, 52, word);
  }

  template  <>
  void Utils::float_to_word(float f, uint32_t word[])
  {
    _float_to_word( static_cast<double>(f), 8, 23, word);
  }


float Utils::word_to_float(uint32_t w)
{
  float v;
  float addv;
  uint32_t tm = 23, te = 8, excess = 127;
  uint32_t sign, exp, mantissa;

  sign = (w >> 31);
  mantissa = (w & ((1 << tm)-1));
  exp = ((w >> tm) & ((1 << te)-1)) - excess;

  addv = 1.0f;
  v = 1.0;
  for (int i=0; i<tm; i++)
  {
    if ((mantissa >> (tm-i)) & 1)
    {
      v += addv;
    }
    addv /= 2;
  }

  v *= powf(2, exp);

  if (sign == 1)
    v *= -1;

  return v;
}

} /* namespace */
