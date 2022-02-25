#ifndef MIPS_SIM_UTILS_H
#define MIPS_SIM_UTILS_H

#include "global_defs.h"
#include <cstdint>

namespace mips_sim
{

class Utils
{
public:
  static void float_to_word(float f, uint32_t word[]);
  static void double_to_word(double f, uint32_t word[]);

  static float word_to_float(uint32_t word[]);
  static double word_to_double(uint32_t word[]);

  static uint32_t find_instruction_by_name(std::string opname);
  static uint8_t find_register_by_name(std::string regname);

  static std::string decode_instruction(instruction_t instruction);
  static uint32_t encode_instruction(instruction_t instruction);
  static uint32_t assemble_instruction(std::string instruction_str);

  static instruction_t fill_instruction(uint32_t instruction_code);
};

} /* namespace */
#endif
