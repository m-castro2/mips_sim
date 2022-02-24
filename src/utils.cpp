#include "utils.h"
#include <cmath>
#include <cstdio>
#include <cstring>
#include <cassert>
#include <iostream>
#include <sstream>

using namespace std;

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

uint32_t Utils::find_instruction_by_name(string opname)
{
  uint32_t instruction_index = UNDEF32;
  for (uint32_t i=0; i<(sizeof(instructions_def)/sizeof(instruction_format_t)); ++i)
  {
    if (opname == instructions_def[i].opname)
    {
      instruction_index = i;
      break;
    }
  }

  return instruction_index;
}

uint8_t Utils::find_register_by_name(string regname)
{
  uint8_t register_index = UNDEF8;
  for (uint8_t i=0; i<(sizeof(registers_def)/sizeof(register_format_t)); ++i)
  {
    if (regname == registers_def[i].regname_generic ||
        regname == registers_def[i].regname_int ||
        regname == registers_def[i].regname_fp)
    {
      register_index = i;
      break;
    }
  }

  return register_index;
}

string Utils::decode_instruction(instruction_t instruction)
{
  stringstream ss;
  size_t instruction_index;

  if (instruction.code == 0)
    return "nop";

  instruction_index = find_instruction(instruction);

  ss << instructions_def[instruction_index].opname << " ";
  if (instructions_def[instruction_index].format == FORMAT_R)
  {
    if (instruction.funct == SUBOP_SLL || instruction.funct == SUBOP_SRL)
    {
      ss << registers_def[instruction.rd].regname_int << ", ";
      ss << registers_def[instruction.rt].regname_int << ", ";
      ss << static_cast<uint32_t>(instruction.shamt);
    }
    else if (instruction.funct != SUBOP_SYSCALL)
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
  else if (instructions_def[instruction_index].format == FORMAT_J)
  {
    ss << "0x" << hex << instruction.addr_j;
  }
  else
  {
    ss << registers_def[instruction.rt].regname_int << ", ";
    if (instruction.opcode == OP_LW || instruction.opcode == OP_SW ||
        instruction.opcode == OP_LB || instruction.opcode == OP_SB)
    {
      ss << "0x" << hex << instruction.addr_i << "(";
      ss << registers_def[instruction.rs].regname_int << ")";
    }
    else if (instruction.opcode == OP_LUI)
    {
      ss << "0x" << hex << instruction.addr_i;
    }
    else
    {
      ss << registers_def[instruction.rs].regname_int << ", ";
      ss << "0x" << hex << instruction.addr_i;
    }
  }

  return ss.str();
}

uint32_t Utils::encode_instruction(instruction_t instruction)
{
  uint32_t instcode = static_cast<uint32_t>(instruction.opcode << 26);
  if (instruction.opcode == OP_J)
  {
    instcode |= instruction.addr_j;
  }
  else
  {
    instcode |= static_cast<uint32_t>(instruction.rs << 21);
    instcode |= static_cast<uint32_t>(instruction.rt << 16);
    if (instruction.opcode != OP_RTYPE && instruction.opcode != OP_FTYPE)
    {
      instcode |= instruction.addr_i;
    }
    else
    {
      instcode |= static_cast<uint32_t>(instruction.rd << 11);
      instcode |= static_cast<uint32_t>(instruction.shamt << 6);
      instcode |= instruction.funct;
    }
  }
  return instcode;
}

instruction_t Utils::fill_instruction(uint32_t instruction_code)
{
  instruction_t instruction;

  instruction.code = instruction_code;
  instruction.opcode = instruction.code >> 26;

  instruction.fp_op = (instruction.opcode == OP_FTYPE);
  if (instruction.fp_op)
  {
    /* F format fields */
    instruction.cop = (instruction.code >> 21) & 0x1F;
    instruction.rs  = (instruction.code >> 16) & 0x1F;
    instruction.rt  = (instruction.code >> 11) & 0x1F;
    instruction.rd  = (instruction.code >> 6) & 0x1F;
  }
  else
  {
    /* R format fields */
    instruction.rs = (instruction.code >> 21) & 0x1F;
    instruction.rt = (instruction.code >> 16) & 0x1F;
    instruction.rd = (instruction.code >> 11) & 0x1F;
    instruction.shamt = (instruction.code >> 6) & 0x1F;
  }
  instruction.funct = instruction.code & 0x3F;
  /* I format fields */
  instruction.addr_i = instruction.code & 0xFFFF;
  /* J format fields */
  instruction.addr_j = instruction.code & 0x3FFFFFF;

  return instruction;
}

uint32_t Utils::assemble_instruction(string instruction_str)
{
  uint32_t instcode = 0;
  char instruction_cstr[100];
  char * tok;
  instruction_t instruction = {};

  strcpy(instruction_cstr, instruction_str.c_str());
  tok = strtok(instruction_cstr, " ");
  if (tok == nullptr)
    return 0;
  size_t instruction_index = find_instruction_by_name(tok);
  if (instruction_index == UNDEF32)
    return 0;

  instruction_format_t instruction_def = instructions_def[instruction_index];

  instruction.opcode = static_cast<uint8_t>(instruction_def.opcode);
  if (instruction_def.format == FORMAT_R)
  {
    if (!strcmp(tok, "syscall"))
    {
      instruction.funct = SUBOP_SYSCALL;
    }
    else
    {
      instruction.funct = static_cast<uint8_t>(instruction_def.subopcode);
      tok = strtok(nullptr, ", ");
      instruction.rd = find_register_by_name(tok);
      tok = strtok(nullptr, ", ");
      instruction.rs = find_register_by_name(tok);
      tok = strtok(nullptr, ", ");
      instruction.rt = find_register_by_name(tok);
    }
  }
  else if (instruction_def.format == FORMAT_I)
  {
    tok = strtok(nullptr, ",() ");
    instruction.rt = find_register_by_name(tok);
    tok = strtok(nullptr, ",() ");
    if (instruction.opcode == OP_LW || instruction.opcode == OP_SW ||
        instruction.opcode == OP_LB || instruction.opcode == OP_SB)
    {
      if (tok[1] == 'x')
        instruction.addr_i = static_cast<uint16_t>(strtol(tok, nullptr, 16));
      else
        instruction.addr_i = static_cast<uint16_t>(atoi(tok));
      tok = strtok(nullptr, ",() ");
      instruction.rs = find_register_by_name(tok);
    }
    else
    {
      instruction.rs = find_register_by_name(tok);
      tok = strtok(nullptr, ",() ");
      if (tok[1] == 'x')
        instruction.addr_i = static_cast<uint16_t>(strtol(tok, nullptr, 16));
      else
        instruction.addr_i = static_cast<uint16_t>(atoi(tok));
    }
  }
  else if (instruction_def.format == FORMAT_J)
  {
    tok = strtok(nullptr, ", ");
    if (tok[1] == 'x')
      instruction.addr_j = static_cast<uint32_t>(strtol(tok, nullptr, 16));
    else
      instruction.addr_j = static_cast<uint32_t>(atoi(tok));
  }

  instcode = Utils::encode_instruction(instruction);
  return instcode;
}

} /* namespace */
