#ifndef CPU_H
#define CPU_H

#include "control_unit.h"
#include "mem.h"

#define ERROR_UNSUPPORTED_OPERATION 101
#define ERROR_UNSUPPORTED_SUBOPERATION 102

typedef struct {
    uint32_t code;
    uint8_t opcode;
    uint8_t rs;
    uint8_t rt;
    uint8_t rd;
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
#endif
