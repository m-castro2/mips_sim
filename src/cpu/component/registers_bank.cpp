#include "registers_bank.h"
#include "../../exception.h"
#include "../../global_defs.h"
#include "../../utils.h"

#include <iostream>
#include <cassert>

using namespace std;

namespace mips_sim
{

  RegistersBank::RegistersBank() {}
  RegistersBank::~RegistersBank() {}

  /* unformatted operations *************************************************************/

  uint32_t RegistersBank::get(const string key) const
  {
    if (registers.find(key) == registers.end())
      throw Exception::e(REG_UNDEF_KEY_EXCEPTION, "Undefined register key");
    
    return registers.at(key);
  }
  
  uint32_t RegistersBank::at(const uint8_t reg_id) const
  {
    if (reg_id >= register_names.size())
      throw Exception::e(REG_UNDEF_EXCEPTION, "Undefined register", reg_id);

    const string reg_name = register_names.at(reg_id);
    return registers.at(reg_name);
  }
  
  string RegistersBank::hex32_get(const string key, const int length) const
  {
    uint32_t reg_value = get(key);
    return Utils::hex32(reg_value, length);
  }

  string RegistersBank::hex32_at(const uint8_t reg_id, const int length) const
  {
    uint32_t reg_value = at(reg_id);
    return Utils::hex32(reg_value, length);
  }

  void RegistersBank::set(const string key, const uint32_t value)
  {
    if (registers.find(key) == registers.end())
      throw Exception::e(REG_UNDEF_KEY_EXCEPTION, "Undefined register key");
    
    registers[key] = value;
  }
  
  void RegistersBank::set_at(const uint8_t reg_id, const uint32_t value)
  {
    if (reg_id >= register_names.size())
      throw Exception::e(REG_UNDEF_EXCEPTION, "Undefined register", reg_id);
    
    registers[register_names[reg_id]] = value;
  }
  
  /* formatted operations (float) *******************************************************/

  float RegistersBank::read_float(const string key) const
  {
    uint32_t value = get(key);
    return Utils::word_to_float(&value);
  }

  float RegistersBank::read_float_at(const uint8_t reg_id) const
  {
    uint32_t value = at(reg_id);
    return Utils::word_to_float(&value);
  }

  void RegistersBank::write_float(const string key, float value)
  {
    uint32_t word;
    Utils::float_to_word(value, &word);

    set(key, word);
  }

  void RegistersBank::write_float_at(const uint8_t reg_id, float value)
  {
    uint32_t word;
    Utils::float_to_word(value, &word);

    set_at(reg_id, word);
  }

  /* formatted operations (double) ******************************************************/

  double RegistersBank::read_double(const string key) const
  {
    long position = distance(begin(registers), registers.find(key));
    assert(position < 31);
    uint32_t value[2] = { at(static_cast<uint8_t>(position)), at(static_cast<uint8_t>(position) + 1) };
    return Utils::word_to_double(value);
  }

  double RegistersBank::read_double_at(const uint8_t reg_id) const
  {
    uint32_t value[2] = { at(reg_id), at(reg_id + 1) };
    return Utils::word_to_double(value);
  }
      
  void RegistersBank::write_double(const string key, double value)
  {
    long position = distance(begin(registers), registers.find(key));
    assert(position < 31);
    uint32_t rvalue[2];
    Utils::double_to_word(value, rvalue);

    set_at(static_cast<uint8_t>(position), rvalue[0]);
    set_at(static_cast<uint8_t>(position) + 1, rvalue[1]);
  }

  void RegistersBank::write_double_at(const uint8_t reg_id, double value)
  {
    uint32_t rvalue[2];
    Utils::double_to_word(value, rvalue);

    set_at(reg_id, rvalue[0]);
    set_at(reg_id + 1, rvalue[1]);
  }

  void RegistersBank::reset( void )
  {
    for (auto& [_, v] : registers)
      v = 0;
  }

  /***************************************************************************************/
  
  GPRegistersBank::GPRegistersBank()
  {
    for (uint8_t i=0; i<(sizeof(registers_def)/sizeof(register_format_t)); ++i)
    {
      string regname = registers_def[i].regname_int;
      register_names.push_back(regname);
      
      registers[regname] = 0;
    }
  }
  
  /***************************************************************************************/
  
  FPRegistersBank::FPRegistersBank()
  {
    for (uint8_t i=0; i<(sizeof(registers_def)/sizeof(register_format_t)); ++i)
    {
      string regname = registers_def[i].regname_fp;
      register_names.push_back(regname);
      
      registers[regname] = 0;
    }
  }
  
  /***************************************************************************************/
  
  SpecialRegistersBank::SpecialRegistersBank()
  {
    for (uint8_t i=0; i<(sizeof(registers_def)/sizeof(register_format_t)); ++i)
    {
      string regname = registers_def[i].regname_special;
      
      if (regname.length() == 0)
        break;
        
      register_names.push_back(regname);
      
      registers[regname] = 0;
    }
  }
  
} /* namespace */
