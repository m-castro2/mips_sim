#ifndef MIPS_SIM_CPU_H
#define MIPS_SIM_CPU_H

#include "component/alu.h"
#include "component/control_unit.h"
#include "component/registers_bank.h"
#include "../global_defs.h"
#include "../mem.h"

#include <map>
#include <memory>
#include <vector>
#include <iostream>

#define ERROR_UNSUPPORTED_OPERATION    101
#define ERROR_UNSUPPORTED_SUBOPERATION 102

#define DEFAULT_FP_ADD_DELAY 2
#define DEFAULT_MULT_DELAY   4
#define DEFAULT_DIV_DELAY    6

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

  uint32_t read_register( uint8_t reg_index) const;
  uint32_t read_fp_register( uint8_t reg_index) const;
  float read_register_f( uint8_t reg_index) const;
  double read_register_d( uint8_t reg_index) const;
  uint32_t read_special_register(std::string reg_name) const;

  virtual void reset( bool reset_data_memory = true,
                      bool reset_text_memory = true );
                      
  virtual const std::map<std::string, int> get_status() const;
  virtual bool set_status(std::map<std::string, int> new_status, bool merge = false);

  virtual bool next_cycle( std::ostream &out = std::cout );
  virtual void print_diagram( std::ostream &out = std::cout ) const;
  virtual void print_status( std::ostream &out = std::cout ) const = 0;
  uint32_t get_cycle( void ) const;
  bool run_to_cycle( uint32_t cycle, std::ostream &out = std::cout );

  const std::vector<uint32_t> & get_loaded_instructions();
  
protected:

  void syscall( uint32_t value );

  void write_register( uint8_t reg_index, uint32_t value);
  void write_fp_register( uint8_t reg_index, uint32_t value);
  void write_register_f( uint8_t reg_index, float value);
  void write_register_d( uint8_t reg_index, double value);

  std::shared_ptr<Memory> memory;
  std::shared_ptr<ControlUnit> control_unit;

  /* status */
  uint32_t cycle;
  size_t mi_index;
  bool ready;

    /* stall cycles */
  int execution_stall;

  std::vector<uint32_t> loaded_instructions;
  std::map<std::string, int> status;

  /* components */
  std::unique_ptr<Alu> alu; /* Arithmetic Logic Unit */
  std::shared_ptr<GPRegistersBank> gpr_bank; /* general purpose registers */
  std::shared_ptr<FPRegistersBank> fpr_bank; /* floating point registers */
  std::shared_ptr<SpecialRegistersBank> sr_bank;  /* special registers */

private:

  std::string register_str(uint8_t reg_id, bool fp,
                           bool show_value, bool show_double) const;
};

} /* namespace */
#endif
