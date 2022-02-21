#include "cpu/control_unit.h"
#include "mem.h"
#include "cpu/cpu_multi.h"
#include "utils.h"

#include <iostream>
#include <cassert>

#include <iomanip>
#include <bitset>
#include <memory>

using namespace mips_sim;

void load_program(char *program_filename, Memory & memory);

int main(int argc, char * argv[])
{

  if (argc < 2)
  {
    std::cout << "Call " << argv[0] << " filename" << std::endl;
    return 0;
  }

  ControlUnit cu(uc_signals_multi, uc_microcode_multi, uc_ctrl_dir_multi);
  cu.print_microcode();

  std::shared_ptr<Memory> mem = std::shared_ptr<Memory>(new Memory());

  std::cout << "Reading " << argv[1] << std::endl;
  load_program(argv[1], *mem);
  std::cout << "Done" << std::endl;

  CpuMulti cpu(cu, mem);

  for (size_t i = 0; i<4; i++)
  {
    std::cout << "------------------------------------------ cycle " << std::dec << i << std::endl;
    cpu.next_cycle();

    if (cpu.PC < 0x00400000)
      break;
  }

  return 0;
}

void load_program(char *program_filename, Memory & memory)
{
  FILE * prog;
  size_t i;
  uint32_t word;

  /* Open program file. */
  prog = fopen(program_filename, "r");
  if (!prog) {
    printf("Error: Can't open program file %s\n", program_filename);
    exit(-1);
  }

  /* Read in the program. */

  i = 0;
  while (fscanf(prog, "%x\n", &word) != EOF) {
    memory.mem_write_32(static_cast<uint32_t>(MEM_TEXT_START + i), word);
    i += 4;
  }

  fclose(prog);

  printf("Read %ld words from program into memory: 0x%08x to 0x%08x\n\n", i/4, static_cast<uint32_t>(MEM_TEXT_START), static_cast<uint32_t>(MEM_TEXT_START+i));
}
