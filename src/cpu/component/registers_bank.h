#ifndef MIPS_SIM_REGISTERS_BANK_H
#define MIPS_SIM_REGISTERS_BANK_H

#include <map>
#include <vector>
#include <string>
#include <cstdint>

namespace mips_sim
{

  class RegistersBank
  {
    public:
    
      RegistersBank();
      ~RegistersBank();

      /* unformatted operations */

      uint32_t get(const std::string key) const;
      uint32_t at(const uint8_t reg_id) const;
      std::string hex32_get(const std::string key, const int length=8) const;
      std::string hex32_at(const uint8_t reg_id, const int length=8) const;

      void set(const std::string key, const uint32_t value);
      void set_at(const uint8_t reg_id, const uint32_t value);

      /* formatted operations (float) */

      float read_float(const std::string key) const;
      float read_float_at(const uint8_t reg_id) const;

      void write_float_at(const uint8_t reg_id, float value);
      void write_float(const std::string key, float value);

      /* formatted operations (double) */

      double read_double(const std::string key) const;
      double read_double_at(const uint8_t reg_id) const;

      void write_double(const std::string key, double value);
      void write_double_at(const uint8_t reg_id, double value);

      /* set all registers to 0 (or default values) */
      void reset( void );

    protected:
    
      std::map<std::string, int> registers;
      std::vector<std::string> register_names;
  };
  
  /* general purpose registers */
  class GPRegistersBank : public RegistersBank
  {
    public:
    
      GPRegistersBank();
  };
  
  /* floating-point registers */
  class FPRegistersBank : public RegistersBank
  {
    public:
    
      FPRegistersBank();
  };
  
  /* special registers */
  class SpecialRegistersBank : public RegistersBank
  {
    public:
    
      SpecialRegistersBank();
  };
} /* namespace */
#endif
