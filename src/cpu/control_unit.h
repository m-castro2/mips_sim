#ifndef MIPS_SIM_CONTROL_UNIT_H
#define MIPS_SIM_CONTROL_UNIT_H

#include <string>

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
typedef enum
{
  SUBOP_SLL     = 0x0,
  SUBOP_SRL     = 0x2,
  SUBOP_SRA     = 0x3,
  SUBOP_SLLV    = 0x4,
  SUBOP_SRLV    = 0x6,
  SUBOP_SRAV    = 0x7,
  SUBOP_JR      = 0x8,
  SUBOP_JALR    = 0x9,
  SUBOP_SYSCALL = 0xc,
  SUBOP_MULT    = 0x18,
  SUBOP_MULTU   = 0x19,
  SUBOP_DIV     = 0x1a,
  SUBOP_DIVU    = 0x1b,
  SUBOP_ADD     = 0x20,
  SUBOP_ADDU    = 0x21,
  SUBOP_SUB     = 0x22,
  SUBOP_SUBU    = 0x23,
  SUBOP_AND     = 0x24,
  SUBOP_OR      = 0x25,
  SUBOP_XOR     = 0x26,
  SUBOP_NOR     = 0x27,
  SUBOP_SLT     = 0x2A,
  SUBOP_SLTU    = 0x2B,
  SUBOP_MFHI    = 0x10,
  SUBOP_MTHI    = 0x11,
  SUBOP_MFLO    = 0x12,
  SUBOP_MTLO    = 0x13
} subopcode_t;

typedef enum
{
  SUBOP_FPADD = 0x0,
  SUBOP_FPSUB = 0x1,
  SUBOP_FPMUL = 0x2,
  SUBOP_FPDIV = 0x3
} fpsubopcode_t;

/* special sub-opcodes (opcode=0) */
/*
#define OP_SPECIAL 0x00
#define SUBOP_SLL 0x0
#define SUBOP_SRL 0x2
#define SUBOP_SRA 0x3
#define SUBOP_SLLV 0x4
#define SUBOP_SRLV 0x6
#define SUBOP_SRAV 0x7
#define SUBOP_JR   0x8
#define SUBOP_JALR 0x9
#define SUBOP_SYSCALL 0xc
#define SUBOP_MULT 0x18
#define SUBOP_MULTU 0x19
#define SUBOP_DIV  0x1a
#define SUBOP_DIVU 0x1b
#define SUBOP_ADD  0x20
#define SUBOP_ADDU 0x21
#define SUBOP_SUB  0x22
#define SUBOP_SUBU 0x23
#define SUBOP_AND  0x24
#define SUBOP_OR   0x25
#define SUBOP_XOR  0x26
#define SUBOP_NOR  0x27
#define SUBOP_SLT  0x2A
#define SUBOP_SLTU 0x2B

#define SUBOP_MFHI 0x10
#define SUBOP_MTHI 0x11
#define SUBOP_MFLO 0x12
#define SUBOP_MTLO 0x13
*/

/* primary opcodes */
#define OP_COUNT 24

typedef enum
{
  OP_RTYPE = 0x00,
  OP_J     = 0x02,
  OP_JAL   = 0x03,
  OP_BEQ   = 0x04,
  OP_BNE   = 0x05,
  OP_BLEZ  = 0x06,
  OP_BGTZ  = 0x07,
  OP_ADDI  = 0x08,
  OP_ADDIU = 0x09,
  OP_SLTI  = 0x0a,
  OP_SLTIU = 0x0b,
  OP_ANDI  = 0x0c,
  OP_ORI   = 0x0d,
  OP_XORI  = 0x0e,
  OP_LUI   = 0x0f,
  OP_FP    = 0x11,
  OP_LB    = 0x20,
  OP_LH    = 0x21,
  OP_LW    = 0x23,
  OP_LBU   = 0x24,
  OP_LHU   = 0x25,
  OP_SB    = 0x28,
  OP_SH    = 0x29,
  OP_SW    = 0x2b
} opcode_t;

/*
#define OP_RTYPE 0x00
#define OP_J     0x02
#define OP_JAL   0x03
#define OP_BEQ   0x04
#define OP_BNE   0x05
#define OP_BLEZ  0x06
#define OP_BGTZ  0x07
#define OP_ADDI  0x08
#define OP_ADDIU 0x09
#define OP_SLTI  0x0a
#define OP_SLTIU 0x0b
#define OP_ANDI  0x0c
#define OP_ORI   0x0d
#define OP_XORI  0x0e
#define OP_LUI   0x0f
#define OP_ADDF  0x11
#define OP_LB    0x20
#define OP_LH    0x21
#define OP_LW    0x23
#define OP_LBU   0x24
#define OP_LHU   0x25
#define OP_SB    0x28
#define OP_SH    0x29
#define OP_SW    0x2b
*/

#define SIGNAL_COUNT 14

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
  SIG_BRANCH    = 13
} signal_t;

typedef struct
{
  int opcode;
  int jump1;
  int jump2;
  int jump4;
} ctrl_dir_t;

const std::string signal_names[] = {
 "PCWrite",  "PCSrc",  "IoD", "MemRead", "MemWrite", "IRWrite",
 "MemToReg", "RegDst", "RegWrite", "SelALUA", "SelALUB", "ALUSrc",
 "ALUOp", "Branch"
};

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
  {0x00,  6, -1,  7}, // OP_RTYPE
  {0x02,  9, -1, -1}, // OP_J
  {0x03, -1, -1, -1}, // OP_JAL
  {0x04,  8, -1, -1}, // OP_BEQ
  {0x05,  8, -1, -1}, // OP_BNE
  {0x06, -1, -1, -1}, // OP_BLEZ
  {0x07, -1, -1, -1}, // OP_BGTZ
  {0x08, 10, -1,  7}, // OP_ADDI
  {0x09, 10, -1,  7}, // OP_ADDIU
  {0x0a, 10, -1,  7}, // OP_SLTI
  {0x0b, 10, -1,  7}, // OP_SLTIU
  {0x0c, 10, -1,  7}, // OP_ANDI
  {0x0d, 10, -1,  7}, // OP_ORI
  {0x0e, 10, -1,  7}, // OP_XORI
  {0x0f, 10, -1,  7}, // OP_LUI
  {0x20, -1, -1, -1}, // OP_LB
  {0x21, -1, -1, -1}, // OP_LH
  {0x23,  2,  3, -1}, // OP_LW
  {0x24, -1, -1, -1}, // OP_LBU
  {0x25, -1, -1, -1}, // OP_LHU
  {0x28, -1, -1, -1}, // OP_SB
  {0x29, -1, -1, -1}, // OP_SH
  {0x2b,  2,  5, -1}
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
