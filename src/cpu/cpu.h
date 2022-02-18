#ifndef MIPS_SIM_CPU_H
#define MIPS_SIM_CPU_H

#include "control_unit.h"
#include "../mem.h"

namespace mips_sim
{

#define ERROR_UNSUPPORTED_OPERATION 101
#define ERROR_UNSUPPORTED_SUBOPERATION 102

/*
 * R format: opcode(6) rs(5) rt(5) rd(5) shamt(5) funct(6)
 * I format: opcode(6) rs(5) rt(5) imm(16)
 * J format: opcode(6) addr(6)
 * F format: opcode(6) cop(5) rs(5) rt(5) rd(5) funct(6)
 */
typedef struct {
    uint32_t code;
    bool fp_op; // FP instruction
    uint8_t opcode;
    uint8_t cop; // for FP instructions
    uint8_t rs; // or fs
    uint8_t rt; // or ft
    uint8_t rd; // or fd
    uint8_t shamt;
    uint8_t funct;
    uint16_t addr_i;
    uint32_t addr_j;
} instruction_t;

class Cpu
{
public:
  Cpu(ControlUnit & cu, Memory & memory);

  void next_cycle( void );

private:
  ControlUnit cu;
  Memory memory;

  size_t cycle;
  int mi_index;

  uint32_t PC;
  uint32_t HI, LO;

  uint32_t A_REG;
  uint32_t B_REG;
  uint32_t ALU_OUT_REG;
  uint32_t MEM_DATA_REG;

  uint32_t gpr[32];
  float fpr[32];

  instruction_t instruction;

  void syscall( uint32_t value );
};

} /* namespace */
#endif
