#ifndef MIPS_SIM_CONTROL_UNIT_H
#define MIPS_SIM_CONTROL_UNIT_H

#include "../global_defs.h"

#include <vector>

#define X -1
#define MAX_MICROINSTRUCTIONS 32

namespace mips_sim
{

// SIMULA:
// https://www.sciencedirect.com/topics/computer-science/floating-point-register
// FTYPE
// OP(6) COP(5) FT(5) FS(5) FD(5) FUNCT(6)
// add.s Opcode = 0x11 (17) r0 rs rt rd func(0)
// add.d Opcode = 0x11 (17) r1 rs rt rd func(0)
// sub.s Opcode = 0x11 (17) r0 rs rt rd func(1)
// sub.d Opcode = 0x11 (17) r1 rs rt rd func(1)
// mul.s Opcode = 0x11 (17) r0 rs rt rd func(2)
// mul.d Opcode = 0x11 (17) r1 rs rt rd func(2)
// div.s Opcode = 0x11 (17) r0 rs rt rd func(3)
// div.d Opcode = 0x11 (17) r1 rs rt rd func(3)

/* special sub-opcodes (opcode=0) */

typedef struct
{
  int opcode;
  size_t jump1;
  size_t jump2;
  size_t jump4;
} ctrl_dir_t;

#define SIGNAL_COUNT 15

typedef enum
{
  SIG_PCWRITE   = 0,
  SIG_PCSRC     = 1,
  SIG_IOD       = 2,
  SIG_MEMREAD   = 3,
  SIG_MEMWRITE  = 4,
  SIG_IRWRITE   = 5,
  SIG_MEM2REG   = 6,
  SIG_REGDST    = 7,
  SIG_REGWRITE  = 8,
  SIG_SELALUA   = 9,
  SIG_SELALUB   = 10,
  SIG_ALUSRC    = 11,
  SIG_ALUOP     = 12,
  SIG_BRANCH    = 13,
  SIG_CTRLDIR   = 14
} signal_t;

class ControlUnit
{
public:

  ControlUnit(const uint32_t uc_signal_bits[SIGNAL_COUNT],
              const int uc_microcode_matrix[][SIGNAL_COUNT],
              const ctrl_dir_t * uc_ctrl_dir);

  static std::vector<uint32_t> build_microcode(const int [][SIGNAL_COUNT],
                                               const uint32_t signals[SIGNAL_COUNT]);

  uint32_t test(uint32_t state, signal_t signal) const;

  void set(uint32_t & state, signal_t signal, int value = -1) const;

  uint32_t get_microinstruction(size_t index) const;

  size_t get_next_microinstruction(size_t index, int opcode) const;

  void print_microcode( void ) const;

  void print_microinstruction( size_t index ) const;

  uint32_t get_signal_bitmask( signal_t const signal[], size_t count ) const;

private:
  uint32_t uc_signals[SIGNAL_COUNT];
  std::vector<uint32_t> uc_microcode;
  ctrl_dir_t uc_ctrl_dir[OP_COUNT];

  bool ctrl_dir_set;
};

} /* namespace */
#endif
