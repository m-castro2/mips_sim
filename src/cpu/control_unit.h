#ifndef MIPS_SIM_CONTROL_UNIT_H
#define MIPS_SIM_CONTROL_UNIT_H

#include "../global_defs.h"

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
  int jump1;
  int jump2;
  int jump4;
} ctrl_dir_t;

const int microcode_multi[12][SIGNAL_COUNT + 1] =
// B   A   A   A   R   R   M   I   M   M   I   P   P
// r   O   L   L   w   d   2   R   w   r   o   C   C
// n   p   b   a   r   s   R   w   r   d   D   s   w
// -   2   2   1   1   1   1   1   1   1   1   2   1
 {{X,  0,  1,  0,  X,  X,  X,  1,  X,  1,  0,  0,  1,     3 }, //0  0000 1000 0101 0001
  {X,  0,  3,  0,  X,  X,  X,  X,  X,  X,  X,  X,  X,     1 }, //1  0001 1000 0000 0000
// Mem
  {X,  0,  2,  1,  X,  X,  X,  X,  X,  X,  X,  X,  X,     2 }, //2  0001 0100 0000 0000
// Lw2
  {X,  X,  X,  X,  X,  X,  X,  X,  X,  1,  1,  X,  X,     3 }, //3  0000 0000 0001 1000
  {X,  X,  X,  X,  1,  0,  1,  X,  X,  X,  X,  X,  X,     0 }, //4  0000 0010 1000 0000
// Sw2
  {X,  X,  X,  X,  X,  X,  X,  X,  1,  X,  1,  X,  X,     0 }, //5  0000 0000 0010 1000
// Rformat2
  {X,  2,  0,  1,  X,  X,  X,  X,  X,  X,  X,  X,  X,     4 }, //6  0100 0100 0000 0000
// Arit3
  {X,  X,  X,  X,  1,  1,  0,  X,  X,  X,  X,  X,  X,     0 }, //7  0000 0011 0000 0000
// Beq
  {1,  1,  0,  1,  X,  X,  X,  X,  X,  X,  X,  1,  1,     0 }, //8  1010 0100 0000 0011
// Jump1
  {X,  X,  X,  X,  X,  X,  X,  X,  X,  X,  X,  2,  1,     0 }, //9  0000 0000 0000 0101
// I-type2
  {X,  2,  2,  1,  X,  X,  X,  X,  X,  X,  X,  X,  X,     3 }, //10 0101 0100 0000 0000
  {X,  X,  X,  X,  1,  0,  0,  X,  X,  X,  X,  X,  X,     0 }  //11 0000 0010 0000 0000
};

const ctrl_dir_t uc_ctrl_dir_multi[OP_COUNT] =
{
  {OP_RTYPE,  6, -1,  7},
  {OP_J,      9, -1, -1},
  {OP_JAL,   -1, -1, -1},
  {OP_BEQ,    8, -1, -1},
  {OP_BNE,    8, -1, -1},
  {OP_BLEZ,  -1, -1, -1},
  {OP_BGTZ,  -1, -1, -1},
  {OP_ADDI,  10, -1,  7},
  {OP_ADDIU, 10, -1,  7},
  {OP_SLTI,  10, -1,  7},
  {OP_SLTIU, 10, -1,  7},
  {OP_ANDI,  10, -1,  7},
  {OP_ORI,   10, -1,  7},
  {OP_XORI,  10, -1,  7},
  {OP_LUI,   10, -1,  7},
  {OP_LB,    -1, -1, -1},
  {OP_LH,    -1, -1, -1},
  {OP_LW,     2,  3, -1},
  {OP_LBU,   -1, -1, -1},
  {OP_LHU,   -1, -1, -1},
  {OP_SB,    -1, -1, -1},
  {OP_SH,    -1, -1, -1},
  {OP_SW,     2,  5, -1}
};

const uint32_t uc_signals_multi[SIGNAL_COUNT] =
  { 1, 2, 1,  1, 1, 1, 1,  1, 1, 1, 2, 0, 2, 1 };

// CtrlDir(4) -(12)
// Branch(1), AluOp(2),
// AluB(2), AluA(1), RegWrite(1)
// RegDest(1), Mem2Reg(1), IRWrite(1), MemWrite(1)
// MemRead(1), IoD(1), PCSrc(2), PCWrite(1)
const uint32_t uc_microcode_multi[MAX_MICROINSTRUCTIONS] =
  { 0x30000851,
    0x10001800,
    0x20001400, // Mem1
    0x30000018, // Lw2
    0x00000280,
    0x00000028, // Sw2
    0x40004400, // Rformat
    0x00000300, // Arit3
    0x0000A403, // Beq1
    0x00000005, // Jump1
    0x30005400, // Iformat
    0x00000200,
  };

const uint32_t uc_signals_pipelined[SIGNAL_COUNT] =
  { 0, 1, 0, 1, 1, 0, 1, 1, 1, 0, 0, 2, 2, 1 };

class ControlUnit
{
public:

  ControlUnit(const uint32_t * uc_signal_bits, const uint32_t * uc_microcode, const ctrl_dir_t * uc_ctrl_dir);

  uint32_t test(uint32_t state, signal_t signal) const;

  void set(uint32_t & state, signal_t signal, int value = -1) const;

  uint32_t get_microinstruction(int index) const;

  int get_next_microinstruction(int index, int opcode) const;

  void print_microcode( void ) const;

  void print_microinstruction( int index ) const;

private:
  uint32_t uc_signals[SIGNAL_COUNT];
  uint32_t uc_microcode[MAX_MICROINSTRUCTIONS];
  ctrl_dir_t uc_ctrl_dir[OP_COUNT];
};

} /* namespace */
#endif
