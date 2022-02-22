/* Code derived from James Mak's MIPS assembler
 */
#ifndef MIPS_SIM_ASSEMBLER_H
#define MIPS_SIM_ASSEMBLER_H

#include "../mem.h"
#include <memory>
#include <string>
#include <vector>

struct Labels{
  std::string name;
  uint32_t address;
};

namespace mips_sim
{

class Assembler
{
public:
  Assembler();

  /* read the file and parse symbols  */
  int open_file(std::string filename);

  /* organize the symbols vector */
  void setup_symbols();

  void setup_code();

  uint32_t load_code(std::shared_ptr<Memory> memory);

  /* print out the assembled instructions */
  void print_file(std::string output_file);

private:
  /* pop an immediate value from symbols vector */
  uint16_t pop_immediate(uint32_t &numberOfSymbols, uint32_t &symbolCounter);
  /* pop a branch offset from symbols vector */
  uint16_t pop_branch_offset(uint32_t &numberOfSymbols, uint32_t &symbolCounter,
                             uint32_t &labelsCounter, uint32_t &lineCounter);
  /* pop a register ID from symbols vector */
  uint8_t pop_register(uint32_t &numberOfSymbols, uint32_t &symbolCounter,
                       uint32_t &lineCounter);
  /* pop a shift value from symbols vector */
  uint8_t pop_shift(uint32_t &numberOfSymbols, uint32_t &symbolCounter);
  /* pop a jump address from symbols vector */
  uint32_t pop_jump_address(uint32_t &numberOfSymbols, uint32_t &symbolCounter,
                            uint32_t &labelsCounter);
  /* pop a memory offset from symbols vector */
  uint16_t pop_mem_offset(uint32_t &numberOfSymbols, uint32_t &symbolCounter);

  uint32_t build_instruction(uint32_t &addr,
                             uint32_t &numberOfSymbols, uint32_t &symbolCounter,
                             uint32_t &labelsCounter, uint32_t &lineCounter);

  void first_pass(uint32_t &numberOfSymbols, uint32_t &symbolsCounter,
                  uint32_t &lineCounter, uint32_t &labelsCounter);

  /* consume identifiers (instructions/tags) */
  void consume_identifier(char &value, uint32_t &i);
  /* consume registers */
  void consume_register(char &value, uint32_t &i);
  /* consume digits (decimal/hex) */
  void consume_digit(char &value, uint32_t &i);

  std::vector<Labels> label;
  uint32_t token_size = 0;
  char token;
  std::vector<char> tokens;
  std::vector<std::string> symbols;
  std::vector<uint32_t> instructions;
};

} /* namespace */

#endif
