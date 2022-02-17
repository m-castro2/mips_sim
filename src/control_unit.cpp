#include "control_unit.h"

#include <iostream>
#include <cassert>

#include <iomanip>
#include <bitset>

static inline uint32_t ctz(uint32_t v)
{
  return static_cast<uint32_t>(__builtin_ctz(v));
}

ControlUnit::ControlUnit(const uint32_t * _uc_signal_bits, const uint32_t * _uc_microcode, const ctrl_dir_t * _uc_ctrl_dir)
{
  /* build uc signals */
  uint32_t bit_count = 0;
  for (size_t i = 0; i < SIGNAL_COUNT; ++i)
  {
    if (_uc_signal_bits[i] > 0)
    {
      /* prevent overflow */
      assert(bit_count < 32 - _uc_signal_bits[i]);
      
      uc_signals[i] = static_cast<uint32_t>(((1<<_uc_signal_bits[i])-1) << bit_count);
      
      std::cout << "Signal " << i << ": " << (1<<_uc_signal_bits[i])-1 << "<<" << bit_count << std::endl;

      bit_count += _uc_signal_bits[i];
    }
    else
      uc_signals[i] = 0;
  }

  for (size_t i = 0; i < MAX_MICROINSTRUCTIONS; ++i)
    uc_microcode[i] = _uc_microcode[i];
    
  for (size_t i = 0; i < SIGNAL_COUNT; ++i)
  {
    std::bitset<16> x(uc_signals[i]);
    std::cout << "Signal " << std::setw(2) << i << ": " << std::setw(10) << signal_names[i] << " " << x << std::endl;    
  }
  
  for (size_t i = 0; i < OP_COUNT; ++i)
    uc_ctrl_dir[i] = _uc_ctrl_dir[i];
}

uint32_t ControlUnit::test(uint32_t state, signal_t signal) const
{
  uint32_t sig_mask = uc_signals[signal];
  return (state & sig_mask) >> ctz(sig_mask);
}

void ControlUnit::set(uint32_t & state, signal_t signal, int value) const
{
  state |= static_cast<uint32_t>(value << ctz(uc_signals[signal]));
}

void ControlUnit::print_microcode( void ) const
{
  for (size_t i = 0; i < SIGNAL_COUNT; ++i)
  {
    std::cout << std::setw(10) << signal_names[i] << " ";
    for (size_t j = 0; j < MAX_MICROINSTRUCTIONS; ++j)
    {
      if (uc_microcode[j] == 0) break;
      std::cout << test(uc_microcode[j], static_cast<signal_t>(i)) << " ";
    }
    std::cout << std::endl;
  }
}

void ControlUnit::print_microinstruction( int index ) const
{
  uint32_t microinstruction = uc_microcode[index];
  for (size_t i = 0; i < SIGNAL_COUNT; ++i)
  {
    uint32_t sigvalue = test(microinstruction, static_cast<signal_t>(i));
    std::cout << std::setw(10) << signal_names[i] << " " << sigvalue << std::endl;
  }
}

uint32_t ControlUnit::get_microinstruction(int index) const
{
  return uc_microcode[index];
}

int ControlUnit::get_next_microinstruction(int index, int opcode) const
{
  int jump_type = (uc_microcode[index] >> 28) & 0xF;
  int mi_index = index;
  ctrl_dir_t ctrl_dir_entry;
  
  //TODO: Hashtable
  for (size_t i=0; i<OP_COUNT; ++i)
  {
    if (uc_ctrl_dir[i].opcode == opcode)
    {
      ctrl_dir_entry = uc_ctrl_dir[i];
      break;
    }
  }
    
  switch (jump_type)
  {
    case 0:
      mi_index = 0;
      break;
    case 1:
      mi_index = ctrl_dir_entry.jump1;
      
      if (mi_index == -1)
      {
          std::cout << "Operation level 1 not supported: " << opcode << std::endl;
          return -1;
      }
      break;
    case 2:
      mi_index = ctrl_dir_entry.jump2;
      
      if (mi_index == -1)
      {
          std::cout << "Operation level 2 not supported: " << opcode << std::endl;
          return -1;
      }
      break;
    case 3:
      /* next */
      mi_index++;
      break;
    case 4:
      mi_index = ctrl_dir_entry.jump4;
      
      if (mi_index == -1)
      {
          std::cout << "Operation level 3 not supported: " << opcode << std::endl;
          return -1;
      }
      //TODO: Check overflow
      break;
  }
  return mi_index;
}
