#include "control_unit.h"

#include <iostream>
#include <cassert>

#include <iomanip>
#include <bitset>

using namespace std;

namespace mips_sim
{

static inline uint32_t ctz(uint32_t v)
{
  return static_cast<uint32_t>(__builtin_ctz(v));
}

ControlUnit::ControlUnit(const uint32_t _uc_signal_bits[SIGNAL_COUNT],
                         const int _uc_microcode_matrix[][SIGNAL_COUNT],
                         const ctrl_dir_t * _uc_ctrl_dir)
{
  /* build uc signals */
  uint32_t bit_count = 0;
  ctrl_dir_set = false;

  for (size_t i = 0; i < SIGNAL_COUNT; ++i)
  {
    if (_uc_signal_bits[i] > 0)
    {
      /* prevent overflow */
      assert(bit_count < 32 - _uc_signal_bits[i]);

      uc_signals[i] = static_cast<uint32_t>(((1<<_uc_signal_bits[i])-1) << bit_count);

      // cout << "Signal " << i << ": " << (1<<_uc_signal_bits[i])-1 << "<<" << bit_count << endl;

      bit_count += _uc_signal_bits[i];
    }
    else
      uc_signals[i] = 0;
  }

  uc_microcode = build_microcode(_uc_microcode_matrix, _uc_signal_bits);

  for (size_t i = 0; i < SIGNAL_COUNT; ++i)
  {
    bitset<16> x(uc_signals[i]);
    if (uc_signals[i] > 0)
    cout << "Signal " << setfill('0') << setw(2) << i << ": "
              << setfill(' ') << setw(10) << signal_names[i] << " "
              << x << endl;
  }

  if (_uc_ctrl_dir != nullptr)
  {
    ctrl_dir_set = true;
    for (size_t i = 0; i < OP_COUNT; ++i)
      uc_ctrl_dir[i] = _uc_ctrl_dir[i];
  }
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
  int l = 10;
  cout << "           ";
  for (size_t i = 0; i < SIGNAL_COUNT && uc_microcode[i] != 0; ++i)
  {
    cout << i/10 << " ";
    l += 2;
  }
  cout << endl << "           ";
  for (size_t i = 0; i < SIGNAL_COUNT && uc_microcode[i] != 0; ++i)
    cout << i%10 << " ";
  cout << endl << setfill('-') << setw(l) << "" << endl;
  for (size_t i = 0; i < SIGNAL_COUNT; ++i)
  {
    cout << setfill(' ') << setw(10) << signal_names[i] << " ";
    for (size_t j = 0; j < MAX_MICROINSTRUCTIONS; ++j)
    {
      if (uc_microcode[j] == 0) break;
      cout << test(uc_microcode[j], static_cast<signal_t>(i)) << " ";
    }
    cout << endl;
  }
}

void ControlUnit::print_microinstruction( size_t index ) const
{
  uint32_t microinstruction = uc_microcode[index];
  for (size_t i = 0; i < SIGNAL_COUNT; ++i)
  {
    uint32_t sigvalue = test(microinstruction, static_cast<signal_t>(i));
    cout << setfill(' ') << setw(10) << signal_names[i] << " "
              << sigvalue << endl;
  }
}

uint32_t ControlUnit::get_microinstruction(size_t index) const
{
  return uc_microcode[index];
}

size_t ControlUnit::get_next_microinstruction(size_t index, int opcode) const
{
  assert(ctrl_dir_set);

  //int jump_type = (uc_microcode[index] >> 28) & 0xF;
  size_t jump_type = test(uc_microcode[static_cast<size_t>(index)], SIG_CTRLDIR);
  size_t mi_index = index;
  ctrl_dir_t ctrl_dir_entry = {};

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

      if (mi_index == UNDEF32)
      {
          cout << "Operation level 1 not supported: " << opcode << endl;
          return UNDEF32;
      }
      break;
    case 2:
      mi_index = ctrl_dir_entry.jump2;

      if (mi_index == UNDEF32)
      {
          cout << "Operation level 2 not supported: " << opcode << endl;
          return UNDEF32;
      }
      break;
    case 3:
      /* next */
      mi_index++;
      break;
    case 4:
      mi_index = ctrl_dir_entry.jump4;

      if (mi_index == UNDEF32)
      {
          cout << "Operation level 3 not supported: " << opcode << endl;
          return UNDEF32;
      }
      //TODO: Check overflow
      break;
  }
  return mi_index;
}

uint32_t ControlUnit::get_signal_bitmask( signal_t const signals[], size_t count ) const
{
  uint32_t bitmask = 0;

  for (size_t i=0; i<count; ++i)
  {
    signal_t signal = signals[i];
    bitmask |= uc_signals[signal];
  }

  return bitmask;
}

vector<uint32_t> ControlUnit::build_microcode(const int v[][SIGNAL_COUNT],
                                              const uint32_t signals[SIGNAL_COUNT])
{
  vector<uint32_t> microcode;
  microcode.reserve(MAX_MICROINSTRUCTIONS);

  for (size_t i=0; i<MAX_MICROINSTRUCTIONS; ++i)
  {
    uint32_t microinstruction = 0;
    uint32_t next_position = 0;
    for (size_t j=0; j<SIGNAL_COUNT; ++j)
    {
      uint32_t signal_bits = signals[j];
      uint32_t signal_value = static_cast<uint32_t>(v[i][j]>0?v[i][j]:0);

      if (!signal_bits)
        continue;

      microinstruction |= (signal_value << next_position);
      next_position += signal_bits;
    }

    if (!microinstruction)
      break;

    microcode.push_back(microinstruction);
  }
  return microcode;
}

} /* namespace */
