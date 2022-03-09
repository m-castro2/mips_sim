#ifndef MIPS_SIM_CPU_H
#define MIPS_SIM_CPU_H

#include "control_unit.h"
#include "../global_defs.h"
#include "../mem.h"

#include <memory>
#include <vector>
#include <iostream>

#define ERROR_UNSUPPORTED_OPERATION    101
#define ERROR_UNSUPPORTED_SUBOPERATION 102

//TODO: Change into config file
#define FP_ADD_DELAY 2
#define MULT_DELAY 4
#define DIV_DELAY  6

namespace mips_sim
{

class Cpu
{
public:
  Cpu(std::shared_ptr<Memory>, std::shared_ptr<ControlUnit>);
  virtual ~Cpu();

  bool is_ready( void ) const;

  void print_registers( std::ostream &out = std::cout ) const;
  void print_int_registers( std::ostream &out = std::cout ) const;
  void print_fp_registers( std::ostream &out = std::cout ) const;
  uint32_t read_register( size_t reg_index) const;
  uint32_t read_fp_register( size_t reg_index) const;
  float read_register_f( size_t reg_index) const;
  double read_register_d( size_t reg_index) const;

  void reset( bool reset_memory = true );

  virtual bool next_cycle( std::ostream &out = std::cout );
  virtual void print_diagram( std::ostream &out = std::cout ) const;
  bool run_to_cycle( uint32_t cycle );

protected:

  uint32_t alu_compute_op(uint32_t alu_input_a,
                          uint32_t alu_input_b,
                          uint32_t alu_op) const;

  uint32_t alu_compute_subop(uint32_t alu_input_a,
                             uint32_t alu_input_b,
                             uint8_t shift_amount,
                             uint32_t alu_subop);

  void syscall( uint32_t value );

  void write_register( size_t reg_index, uint32_t value);
  void write_fp_register( size_t reg_index, uint32_t value);
  void write_register_f( size_t reg_index, float value);
  void write_register_d( size_t reg_index, double value);

  std::shared_ptr<Memory> memory;
  std::shared_ptr<ControlUnit> control_unit;

  /* status */
  uint32_t cycle;
  size_t mi_index;
  bool ready;

  /* special registers */
  uint32_t HI, LO;
  uint32_t PC;

  /* stall cycles */
  int execution_stall;

  std::vector<uint32_t> loaded_instructions;

private:

  /* register banks */
  uint32_t gpr[32];
  uint32_t fpr[32];

  std::string register_str(size_t reg_id, bool fp,
                           bool show_value, bool show_double) const;
};

} /* namespace */
#endif
