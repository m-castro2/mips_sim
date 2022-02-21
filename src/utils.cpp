#include "utils.h"
#include <cmath>
#include <cstdio>
#include <cassert>
#include <iostream>
#include <sstream>

namespace mips_sim
{

  static void _float_to_word(double f,
                             uint32_t te, uint32_t tm,
                             uint32_t word[]);
  static double _word_to_double(uint32_t w32[],
                                uint32_t tm, uint32_t te);


  template  <>
  void Utils::float_to_word(double f, uint32_t word[])
  {
    _float_to_word(f, 11, 52, word);
  }

  template  <>
  void Utils::float_to_word(float f, uint32_t word[])
  {
    _float_to_word(static_cast<double>(f), 8, 23, word);
  }

template  <>
float Utils::word_to_float(uint32_t w[])
{
  return static_cast<float>(_word_to_double(w, 8, 23));
}

template  <>
double Utils::word_to_float(uint32_t w[])
{
  return _word_to_double(w, 11, 52);
}

/******************************************************************************/

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
      decimal_part -= 1.0;
    }
    decpart <<= static_cast<uint64_t>(1);
  }

  decpart <<= bitcount - tm - 1;
  mantissa = decpart;

  exp = excess;
  if (integer_part > 1)
  {
    while (integer_part > 1)
    {
       mantissa >>= 1;
       mantissa &= (static_cast<uint64_t>(1) << (bitcount-1))-1;
       mantissa |= ((integer_part << (bitcount-1)) & (static_cast<uint64_t>(1)<<(bitcount-1)));
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
  v |= ((exp & ((static_cast<uint64_t>(1)<<te)-1)) << (bitcount-te-1));
  v |= ((mantissa >> (bitcount-tm)) & ((static_cast<uint64_t>(1)<<tm)-1));

  if (bitcount == 64)
  {
    word[1] = static_cast<uint32_t>(v & 0xFFFFFFFF);
    word[0] = static_cast<uint32_t>((v & 0xFFFFFFFF00000000)>>32);
  }
  else
  {
    word[0] = static_cast<uint32_t>(v & 0xFFFFFFFF);
  }
}

static double _word_to_double(uint32_t w32[], uint32_t te, uint32_t tm)
{
  double v;
  double addv;
  uint64_t w = 0;
  uint32_t bitcount = te + tm + 1;
  assert(bitcount == 32 || bitcount == 64);
  uint32_t excess = (1<<(te-1))-1;
  uint64_t sign, exp, mantissa;

  if (bitcount == 32)
  {
    w = w32[0];
  }
  else
  {
    w |= (static_cast<uint64_t>(w32[0]) << 32) + w32[1];
  }
  sign = (w >> (bitcount - 1));
  mantissa = (w & ((static_cast<uint64_t>(1) << tm)-1));
  exp = ((w >> tm) & ((1 << te)-1)) - excess;

  addv = 1.0;
  v = 1.0;
  for (size_t i=0; i<tm; i++)
  {
    if ((mantissa >> (tm-i)) & 1)
    {
      v += addv;
    }
    addv /= 2;
  }
  v *= pow(2, static_cast<double>(exp));

  if (sign == 1)
    v *= -1;

  return v;
}

static size_t find_instruction(instruction_t instruction)
{
  size_t instruction_index = UNDEF32;
  for (size_t i=0; i<(sizeof(instructions_def)/sizeof(instruction_format_t)); ++i)
  {
    if (instruction.opcode == instructions_def[i].opcode)
    {
      if (instruction.opcode == OP_RTYPE || instruction.opcode == OP_FTYPE)
      {
        if (instruction.funct == instructions_def[i].subopcode)
        {
          instruction_index = i;
          break;
        }
      }
      else
      {
        instruction_index = i;
        break;
      }
    }
  }

  assert(instruction_index != UNDEF32);

  return instruction_index;
}

std::string Utils::decode_instruction(instruction_t instruction)
{
  std::stringstream ss;

  size_t instruction_index = find_instruction(instruction);

  ss << instructions_def[instruction_index].opname << " ";
  if (instructions_def[instruction_index].format == FORMAT_R)
  {
    if (instruction.funct != SUBOP_SYSCALL)
    {
      ss << registers_def[instruction.rd].regname_int << ", ";
      ss << registers_def[instruction.rs].regname_int << ", ";
      ss << registers_def[instruction.rt].regname_int;
    }
  }
  else if (instructions_def[instruction_index].format == FORMAT_F)
  {
    ss << registers_def[instruction.rd].regname_fp << ", ";
    ss << registers_def[instruction.rs].regname_fp << ", ";
    ss << registers_def[instruction.rt].regname_fp;
  }
  else if (instructions_def[instruction_index].format == FORMAT_F)
  {
    ss << "0x" << std::hex << instruction.addr_j;
  }
  else
  {
    ss << registers_def[instruction.rt].regname_int << ", ";
    if (instruction.opcode == OP_LW || instruction.opcode == OP_SW ||
        instruction.opcode == OP_LB || instruction.opcode == OP_SB)
    {
      ss << "0x" << std::hex << instruction.addr_i << "(";
      ss << registers_def[instruction.rs].regname_int << ")";
    }
    else
    {
      ss << registers_def[instruction.rs].regname_int << ", ";
      ss << "0x" << std::hex << instruction.addr_i;
    }
  }

  return ss.str();
}

} /* namespace */
