#include "assembler/mips_assembler.h"
#include "assembler/mips_parser.hpp"
#include "cpu/control_unit.h"
#include "cpu/control_unit.h"
#include "cpu/cpu_multi.h"
#include "cpu/cpu_pipelined.h"
#include "mem.h"
#include "utils.h"
#include "exception.h"

#include <iostream>
#include <fstream>
#include <cassert>
#include <cstdio>
#include <iomanip>
#include <bitset>
#include <memory>
#include <vector>

#define MODE_RUN_ASM  1
#define MODE_RUN_HEX  2
#define MODE_ASSEMBLE 3

#define CPU_MULTICYCLE 1
#define CPU_PIPELINED  2

using namespace mips_sim;
using namespace std;

namespace mips_sim
{
uint32_t err_v;
int err_no;
string err_msg;
}

uint32_t load_program(string program_filename, Memory & memory);

int main(int argc, char * argv[])
{
  shared_ptr<Memory> mem = shared_ptr<Memory>(new Memory());
  unique_ptr<Cpu> cpu;

  string input_file, output_file;
  int run_mode = 0, cpu_mode = 0, retval = 0;

  if (argc < 3)
  {
    cerr << "Call " << argv[0] << "{run|runhex|asm} filename [multi|pipe]" << endl;
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
    cerr << "Undefined run mode: " << argv[1] << endl;
    return EXIT_FAILURE;
  }

  cpu_mode = CPU_PIPELINED; /* default mode */
  if (!strcmp(argv[3], "multi"))
    cpu_mode = CPU_MULTICYCLE;
  else if (!strcmp(argv[3], "pipe"))
    cpu_mode = CPU_PIPELINED;

  input_file = argv[2];
  output_file = input_file + ".hex";

  switch(run_mode)
  {
    case MODE_RUN_ASM:
      {
        try
        {
          if (assemble_file(input_file.c_str(), mem) != 0)
          {
            cerr << "Error parsing the input file" << endl;
            exit(EXIT_FAILURE);
          }
        }
        catch(int e)
        {
          cerr << "Error " << e << ": " << err_msg;
          if (err_v)
            cerr << " [0x" << Utils::hex32(err_v) << "]";
          cerr << endl;
          exit(EXIT_FAILURE);
        }
      }
      break;
    case MODE_RUN_HEX:
      {
        uint32_t words_read = load_program(input_file, *mem);
        if (!words_read)
        {
          cerr << "No valid instructions in file " << argv[2] << endl;
          return EXIT_FAILURE;
        }
      }
      break;
    case MODE_ASSEMBLE:
      {
        try
        {
          assemble_file(input_file.c_str(), mem);
          print_file(output_file);
          return EXIT_SUCCESS;
        }
        catch(int e)
        {
          cerr << "Error " << e << ": " << err_msg;
          if (err_v)
            cerr << " [0x" << Utils::hex32(err_v) << "]";
          cerr << endl;
          return EXIT_FAILURE;
        }
      }
    default:
      /* this should never happen */
      assert(0);
      break;
  }

  if (retval != 0)
    return retval;

  switch (cpu_mode)
  {
    case CPU_MULTICYCLE:
      cpu = unique_ptr<Cpu>(new CpuMulti(mem));
      break;
    case CPU_PIPELINED:
      cpu = unique_ptr<Cpu>(new CpuPipelined(mem));
      break;
    default:
      assert(0);
  }

  //ostream nullout(nullptr);
  try
  {
    for (size_t i = 0; cpu->is_ready() ; i++)
    //for (size_t i = 0; i < 13 ; i++)
    {
      //cpu->next_cycle(nullout);
      cpu->next_cycle();
    }
  }
  catch(int e)
  {
    cerr << "EXCEPTION " << e << ": " << err_msg;
    if (err_v)
      cerr << " [0x" << Utils::hex32(err_v) << "]";
    cerr << endl;
  }

  cout << endl << "Registers:" << endl;
  cpu->print_registers(cout);

  cout << endl << "Data Memory:" << endl;
  mem->print_memory(MEM_DATA_START, MEM_DATA_SIZE, cout);

  cout << endl << "Instructions Memory:" << endl;
  mem->print_memory(MEM_TEXT_START, MEM_TEXT_SIZE, cout);

  cout << endl << endl;
  cpu->print_diagram();
  
  return EXIT_SUCCESS;
}

/* load program in memory from file */
uint32_t load_program(string program_filename, Memory & memory)
{
  ifstream prog(program_filename);
  size_t i = 0;
  uint32_t word;
  uint32_t words_read = 0;

  /* Open program file. */
  if (!prog) {
    cerr << "Error: Can't open program file " << program_filename << endl;
    exit(-1);
  }

  /* Read in the program. */

  i = 0;
  while (prog >> hex >> word) { //fscanf(prog, "%x\n", &word) != EOF) {
    cout << "Write " << Utils::hex32(word) << " to " << i << endl;
    memory.mem_write_32(static_cast<uint32_t>(MEM_TEXT_START + i), word);
    i += 4;
    words_read++;
  }

  return words_read;
}
