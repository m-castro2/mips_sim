#include "cpu.h"
#include <cassert>
#include <iostream>

Cpu::Cpu(ControlUnit & _cu, Memory & _memory)
  : cu(_cu), memory(_memory)
{
  PC = MEM_TEXT_START;
  
  cycle = 0;
  mi_index = 0;
  
  /* set register $0 */
  gpr[0] = 0;
}

void Cpu::next_cycle( void )
{
  /* temporary data */
  uint32_t alu_input_a, alu_input_b, alu_output = 0;
  uint32_t word_read = 0;
  
  uint32_t microinstruction = cu.get_microinstruction(mi_index);
  printf("Next microinstruction [%d]: 0x%08x\n", mi_index, microinstruction);
  printf("PC: 0x%08x, A_REG: 0x%08x, B_REG: 0x%08x\n", PC, A_REG, B_REG);

  cu.print_microinstruction(mi_index);
  
  if (cu.test(microinstruction, SIG_MEMREAD))
  {
    uint32_t address;
    if (cu.test(microinstruction, SIG_IOD) == 0)
      address = PC;
    else
      address = ALU_OUT_REG;
    word_read = memory.mem_read_32(address);
  }
  else if (cu.test(microinstruction, SIG_MEMWRITE))
  {
    /* prevent writing instruction data */
    assert(cu.test(microinstruction, SIG_IOD) == 1);
    
    uint32_t address = ALU_OUT_REG;
    memory.mem_write_32(address, B_REG);
  }
  
  /* ALU */
  if (cu.test(microinstruction, SIG_SELALUA) == 0)
      alu_input_a = PC;
  else if (cu.test(microinstruction, SIG_SELALUA) == 1)
      alu_input_a = A_REG;
  else
      assert(0);
            
  if (cu.test(microinstruction, SIG_SELALUB) == 0)
      alu_input_b = B_REG;
  else if (cu.test(microinstruction, SIG_SELALUB) == 1)
      alu_input_b = 4;
  else if (cu.test(microinstruction, SIG_SELALUB) == 2)
  {
      /* 16 bit w sign extension */
      alu_input_b = static_cast<uint32_t>(static_cast<int>(instruction.addr_i) << 16 >> 16);
  }
  else if (cu.test(microinstruction, SIG_SELALUB) == 3)
  {
      alu_input_b = static_cast<uint32_t>(static_cast<int>(instruction.addr_i) << 16 >> 16);
      alu_input_b <<= 2;
  }
  else
      assert(0);
      
  switch (cu.test(microinstruction, SIG_ALUOP))
  {
    case 0:
        alu_output = alu_input_a + alu_input_b;
        break;
    case 1:
        alu_output = alu_input_a - alu_input_b;
        break;
    case 2:
      if (instruction.opcode == OP_RTYPE)
      {
        switch(instruction.funct)
        {
          case SUBOP_SYSCALL:
            /* Syscall */
            syscall(gpr[2]); // call with reg $v0
            break;
          case SUBOP_SLL:
            alu_output = alu_input_b << instruction.shamt; break;
          case SUBOP_SRL:
            alu_output = alu_input_b >> instruction.shamt; break;
          case SUBOP_AND:
            alu_output = alu_input_a & alu_input_b; break;
          case SUBOP_OR:
            alu_output = alu_input_a | alu_input_b; break;
          case SUBOP_XOR:
            alu_output = alu_input_a ^ alu_input_b; break;
          case SUBOP_NOR:
            alu_output = ~(alu_input_a | alu_input_b); break;
          case SUBOP_SLT:
            alu_output = (static_cast<int>(alu_input_a) < static_cast<int>(alu_input_b))?1:0; break;
          case SUBOP_SLTU:
            alu_output = (alu_input_a < alu_input_b)?1:0; break;
          case SUBOP_ADD:
            alu_output = static_cast<uint32_t>(static_cast<int>(alu_input_a) + static_cast<int>(alu_input_b)); break;
          case SUBOP_ADDU:
            alu_output = alu_input_a + alu_input_b; break;
          case SUBOP_SUB:
            alu_output = static_cast<uint32_t>(static_cast<int>(alu_input_a) - static_cast<int>(alu_input_b)); break;
          case SUBOP_SUBU:
            alu_output = alu_input_a + alu_input_b; break;
          default:
            assert(0);
        }
      }
      else
      {
        switch(instruction.opcode)
        {
          case OP_ADDI:
            alu_output = static_cast<uint32_t>(static_cast<int>(alu_input_a) + static_cast<int>(alu_input_b)); break;
          case OP_ADDIU:
            alu_output = alu_input_a + alu_input_b; break;
          case OP_SLTI:
            alu_output = (static_cast<int>(alu_input_a) < static_cast<int>(alu_input_b))?1:0; break;
          case OP_SLTIU:
            alu_output = (alu_input_a < alu_input_b)?1:0; break;
          case OP_ANDI:
            alu_output = alu_input_a & alu_input_b; break;
          case OP_ORI:
            alu_output = alu_input_a | alu_input_b; break;
          case OP_XORI:
            alu_output = alu_input_a ^ alu_input_b; break;
          case OP_LUI:
            alu_output = alu_input_b<<16; break;
          default:
            assert(0);
        }
      }
      break;
     default:
       printf("Undefined ALU operation\n");
       assert(0);
  }
  printf("ALU: 0x%08x op 0x%08x = 0x%08x\n", alu_input_a, alu_input_b, alu_output);
    
  if (cu.test(microinstruction, SIG_PCWRITE))
  {
    bool pcwrite = true;
    if (cu.test(microinstruction, SIG_BRANCH))
    {
      pcwrite = (instruction.opcode == OP_BNE && alu_output > 0) |
                (instruction.opcode == OP_BEQ && alu_output == 0);
    }
    if (pcwrite)
    {
      switch (cu.test(microinstruction, SIG_PCSRC))
      {
      case 0:
        PC = alu_output;
        printf("PC write ALU Out: 0x%08x\n", PC);
        break;
      case 1:
        PC = ALU_OUT_REG;
        printf("PC write ALU Reg: 0x%08x\n", PC);
        break;
      case 2:
        PC = (PC & 0xF0000000) + (instruction.addr_j << 2);
        printf("PC write J: 0x%08x\n", PC);
        break;
      default:
        assert(0);
      }
    }
  }
  
  /* Update registers */
  
  if (cu.test(microinstruction, SIG_IRWRITE))
  {
      instruction.code = word_read;
      instruction.opcode = instruction.code >> 26;
  
      /* R type fields */
      instruction.rs = (instruction.code >> 21) & 0x1F;
      instruction.rt = (instruction.code >> 16) & 0x1F;
      instruction.rd = (instruction.code >> 11) & 0x1F;
      instruction.shamt = (instruction.code >> 6) & 0x1F;
      instruction.funct = instruction.code & 0x3F; 
      /* I type fields */
      instruction.addr_i = instruction.code & 0xFFFF; 
      /* J type fields */
      instruction.addr_j = instruction.code & 0x3FFFFFF;
  
      printf("Instruction: %08x\n", instruction.code);
      printf("IR write: OP=%02x, Rs=%02x, Rt=%02x, Rd=%02x, shamt=%02x, funct=%02x, address16=%04x, address26=%07x\n",
                                instruction.opcode, instruction.rs, instruction.rt, instruction.rd, instruction.shamt,
                                instruction.funct, instruction.addr_i, instruction.addr_j);
  }
  
  if (cu.test(microinstruction, SIG_REGWRITE))
  {
      uint32_t writereg, writedata;
      
      if (cu.test(microinstruction, SIG_REGDST) == 0)
        writereg = instruction.rt;
      else if (cu.test(microinstruction, SIG_REGDST) == 1)
        writereg = instruction.rd;
      else
        assert(0);
              
      if (cu.test(microinstruction, SIG_MEM2REG) == 0)
        writedata = ALU_OUT_REG;
      else if (cu.test(microinstruction, SIG_MEM2REG) == 1)
        writedata = MEM_DATA_REG;
      else
        assert(0);

      gpr[writereg] = writedata;
        
      printf("Register write: Reg=%02x, Data=%08x\n",
                                writereg, writedata);
  }
    
  A_REG = gpr[instruction.rs];
  B_REG = gpr[instruction.rt];
  ALU_OUT_REG = alu_output;
  MEM_DATA_REG = word_read;
  
  /* update MI index */
  mi_index = cu.get_next_microinstruction(mi_index, instruction.opcode);
  if (mi_index < 0)
     exit(ERROR_UNSUPPORTED_OPERATION);
}

void Cpu::syscall( uint32_t value )
{
  switch(value)
  {
    case 1:
      //TODO: print_integer $a0
      break;
    case 2:
      //TODO: print_float $f12
      break;
    case 3:
      //TODO: print_double $f12
      break;
    case 4:
      //TODO: print_string $a0
      break;
    case 5:
      //TODO: read_integer $v0
      break;
    case 6:
      //TODO: read_float $f0
      break;
    case 7:
      //TODO: read_double $f0
      break;
    case 8:
      //TODO: read_string $a0(str), $a1(chars to read)
      break;
    case 9:
      //TODO: sbrk $a0(bytes to allocate) $v0(address)
      break;
    case 10:
      //TODO: Send stop signal or something
      std::cout << "Program done: exiting" << std::endl;
      exit(0);
      break;
    case 41:
      //TODO: random_integer $a0(seed) $a0
      break;
    case 42:
      //TODO: random_int_range $a0(seed), $a1(upper bound) $a0
      break;
    case 43:
      //TODO: random_float $a0(seed) $f0
      break;
    case 44:
      //TODO: random_double $a0(seed) $f0
      break;
    default:
      //TODO: Exception
      std::cout << "Undefined syscall" << std::endl;
      assert(0);
  }
}
