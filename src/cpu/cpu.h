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

  bool is_ready( void ) const;

  void print_registers( void ) const;
  uint32_t read_register( size_t reg_index) const;
  float read_register_f( size_t reg_index) const;
  double read_register_d( size_t reg_index) const;

  void reset( bool reset_memory = true );
  virtual bool next_cycle( bool verbose = true );
  bool run_to_cycle( uint32_t cycle );

  uint32_t PC;

protected:
  
  uint32_t alu_compute_op(uint32_t alu_input_a,
                          uint32_t alu_input_b,
                          uint32_t alu_op) const;

  uint32_t alu_compute_subop(uint32_t alu_input_a,
                             uint32_t alu_input_b,
                             uint32_t alu_subop);

  void write_instruction_register( uint32_t instruction_code );
  void write_register( size_t reg_index, uint32_t value);
  void write_register_f( size_t reg_index, float value);
  void write_register_d( size_t reg_index, double value);

  std::shared_ptr<ControlUnit> control_unit;
  std::shared_ptr<Memory> memory;

  uint32_t cycle;
  size_t mi_index;

  uint32_t HI, LO;

  instruction_t instruction;

  int execution_stall;
  void syscall( uint32_t value );

  bool ready;

private:

  uint32_t gpr[32];
  uint32_t fpr[32];
};

} /* namespace */
#endif
