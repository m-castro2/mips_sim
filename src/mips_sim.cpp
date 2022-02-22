#include "assembler/assembler.h"
#include "cpu/control_unit.h"
#include "cpu/control_unit.h"
#include "cpu/cpu_multi.h"
#include "mem.h"
#include "utils.h"

#include <iostream>
#include <fstream>
#include <cassert>
#include <iomanip>
#include <bitset>
#include <memory>
#include <vector>

#define MODE_RUN_ASM  1
#define MODE_RUN_HEX  2
#define MODE_ASSEMBLE 3

using namespace mips_sim;

uint32_t load_program(std::string program_filename, Memory & memory);

int main(int argc, char * argv[])
{
  std::shared_ptr<Memory> mem = std::shared_ptr<Memory>(new Memory());
  std::shared_ptr<ControlUnit> control_unit;
  std::unique_ptr<Cpu> cpu;

  std::string input_file, output_file;
  int run_mode = 0, retval = 0;

  if (argc != 3)
  {
    std::cout << "Call " << argv[0] << "{run|runhex|asm} filename" << std::endl;
    return 0;
  }

  if (!strcmp(argv[1], "asm"))
    run_mode = MODE_ASSEMBLE;
  else if (!strcmp(argv[1], "run"))
    run_mode = MODE_RUN_ASM;
  else if (!strcmp(argv[1], "runhex"))
    run_mode = MODE_RUN_HEX;
  else
  {
    std::cerr << "Undefined run mode: " << argv[1] << std::endl;
    return EXIT_FAILURE;
  }

  input_file = argv[2];
  output_file = input_file + ".hex";

  switch(run_mode)
  {
    case MODE_RUN_ASM:
      {
        Assembler assm;
        uint32_t words_read;

        if (!(retval = assm.open_file(input_file)))
        {
          assm.setup_symbols();
          assm.setup_code();
          words_read = assm.load_code(mem);
          if (!words_read)
            return EXIT_FAILURE;
        }
      }
      break;
    case MODE_RUN_HEX:
      {
        uint32_t words_read = load_program(input_file, *mem);
        if (!words_read)
        {
          std::cerr << "No valid instructions in file " << argv[2] << std::endl;
          return EXIT_FAILURE;
        }
      }
      break;
    case MODE_ASSEMBLE:
      {
        Assembler assm;

        if (!(retval = assm.open_file(input_file)))
        {
          assm.setup_symbols();
          assm.setup_code();
          assm.print_file(output_file);
        }

        return retval;
      }
      break;
    default:
      /* this should never happen */
      assert(0);
      break;
  }

  if (retval != 0)
    return retval;

  control_unit = std::shared_ptr<ControlUnit>(new ControlUnit(uc_signals_multi, uc_microcode_multi, uc_ctrl_dir_multi));
  control_unit->print_microcode();

  cpu = std::unique_ptr<Cpu>(new CpuMulti(control_unit, mem));

  for (size_t i = 0;; i++)
  {
    std::cout << "------------------------------------------ cycle " << std::dec << i << std::endl;
    cpu->next_cycle();

    if (cpu->PC < 0x00400000)
      break;
  }

  return EXIT_SUCCESS;
}

/* load program in memory from file */
uint32_t load_program(std::string program_filename, Memory & memory)
{
  std::ifstream prog(program_filename);
  size_t i = 0;
  uint32_t word;
  uint32_t words_read = 0;

  /* Open program file. */
  if (!prog) {
    std::cerr << "Error: Can't open program file " << program_filename << std::endl;
    exit(-1);
  }

  /* Read in the program. */

  i = 0;
  while (prog >> std::hex >> word) { //fscanf(prog, "%x\n", &word) != EOF) {
    std::cout << "Write " << std::hex << word << " to " << i << std::endl;
    memory.mem_write_32(static_cast<uint32_t>(MEM_TEXT_START + i), word);
    i += 4;
    words_read++;
  }

  return words_read;
}
