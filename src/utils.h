#ifndef MIPS_SIM_UTILS_H
#define MIPS_SIM_UTILS_H

#include "global_defs.h"
#include <cstdint>

namespace mips_sim
{

class Utils
{
public:
  template  <typename T>
  static void float_to_word(T f, uint32_t word[]);

  template<> void float_to_word(float f, uint32_t word[]);
  template<> void float_to_word(double f, uint32_t word[]);

  template  <typename T>
  static T word_to_float(uint32_t word[]);

  template<> float word_to_float<>(uint32_t word[]);
  template<> double word_to_float<>(uint32_t word[]);

  static uint32_t find_instruction_by_name(std::string opname);
  static uint8_t find_register_by_name(std::string regname);

  static std::string decode_instruction(instruction_t instruction);
  static uint32_t encode_instruction(instruction_t instruction);
  static uint32_t assemble_instruction(std::string instruction_str);

  static instruction_t fill_instruction(uint32_t instruction_code);
};

} /* namespace */
#endif
