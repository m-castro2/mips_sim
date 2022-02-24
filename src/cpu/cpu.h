#ifndef MIPS_SIM_CPU_H
#define MIPS_SIM_CPU_H

#include "control_unit.h"
#include "../global_defs.h"
#include "../mem.h"

#include <memory>

#define ERROR_UNSUPPORTED_OPERATION    101
#define ERROR_UNSUPPORTED_SUBOPERATION 102

//TODO: Change into config file
#define MULT_STALL 4
#define DIV_STALL  6

namespace mips_sim
{

class Cpu
{
public:
  Cpu(std::shared_ptr<ControlUnit>, std::shared_ptr<Memory>);
  virtual ~Cpu();

  virtual bool next_cycle( void ) = 0;
  void write_instruction_register( uint32_t instruction_code );
  void print_registers( void ) const;

  bool is_ready( void ) const;

  uint32_t PC;

protected:
  uint32_t alu_compute_op(uint32_t alu_input_a, uint32_t alu_input_b, uint32_t alu_op);
  uint32_t alu_compute_subop(uint32_t alu_input_a, uint32_t alu_input_b, uint32_t alu_subop);

  std::shared_ptr<ControlUnit> control_unit;
  std::shared_ptr<Memory> memory;

  size_t cycle;
  int mi_index;

  uint32_t HI, LO;

  uint32_t gpr[32];
  uint32_t fpr[32];

  instruction_t instruction;

  int execution_stall;
  void syscall( uint32_t value );

  bool ready;
};

} /* namespace */
#endif
