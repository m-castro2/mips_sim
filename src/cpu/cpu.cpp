#include "cpu.h"
#include <cassert>
#include <iostream>

namespace mips_sim
{

Cpu::Cpu(ControlUnit & _cu, std::shared_ptr<Memory> _memory)
  : cu(_cu), memory(_memory)
{
  PC = MEM_TEXT_START;
  A_REG = UNDEF32;
  B_REG = UNDEF32;
  ALU_OUT_REG = UNDEF32;
  MEM_DATA_REG = UNDEF32;

  instruction.opcode = 0;
  instruction.funct = 0;
  instruction.fp_op = false;

  cycle = 0;
  mi_index = 0;

  execution_stall = 0;

  /* set registers to 0 */
  memset(gpr, 0, 32 * sizeof(int));
  memset(fpr, 0, 32 * sizeof(int));
}

Cpu::~Cpu() {}

uint32_t Cpu::alu_compute_subop(uint32_t alu_input_a, uint32_t alu_input_b, uint32_t alu_op)
{
  uint32_t alu_output = 0xFFFFFFFF;
  switch(alu_op)
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
      alu_output = (static_cast<int32_t>(alu_input_a) < static_cast<int32_t>(alu_input_b))
                   ?1:0;
      break;
    case SUBOP_SLTU:
      alu_output = (alu_input_a < alu_input_b)?1:0; break;
    case SUBOP_ADD:
      alu_output = static_cast<uint32_t>(static_cast<int32_t>(alu_input_a) +
                                         static_cast<int32_t>(alu_input_b));
      break;
    case SUBOP_ADDU:
      alu_output = alu_input_a + alu_input_b; break;
    case SUBOP_SUB:
      alu_output = static_cast<uint32_t>(static_cast<int>(alu_input_a) -
                                         static_cast<int32_t>(alu_input_b));
      break;
    case SUBOP_SUBU:
      alu_output = alu_input_a + alu_input_b; break;
    case SUBOP_MULT:
    {
      uint64_t v = static_cast<uint64_t>(static_cast<int32_t>(alu_input_a) *
                                         static_cast<int32_t>(alu_input_b));
      //TODO: Update HI/LO after stall
      HI = (v >> 32) & 0xFFFFFFFF;
      LO = v & 0xFFFFFFFF;
      execution_stall = MULT_STALL;
    }
      break;
    case SUBOP_MULTU:
    {
      uint64_t v = static_cast<uint64_t>(alu_input_a * alu_input_b);
      //TODO: Update HI/LO after stall
      HI = (v >> 32) & 0xFFFFFFFF;
      LO = v & 0xFFFFFFFF;
      execution_stall = MULT_STALL;
    }
      break;
    case SUBOP_DIV:
    {
      //TODO: Update HI/LO after stall
      /* HI = div, LO = mod */
      HI = static_cast<uint32_t>(static_cast<int32_t>(alu_input_a) / static_cast<int32_t>(alu_input_b));
      LO = static_cast<uint32_t>(static_cast<int32_t>(alu_input_a) % static_cast<int32_t>(alu_input_b));
      execution_stall = DIV_STALL;
    }
      break;
    case SUBOP_DIVU:
    {
      //TODO: Update HI/LO after stall
      /* HI = div, LO = mod */
      HI = alu_input_a / alu_input_b;
      LO = alu_input_a % alu_input_b;
      execution_stall = DIV_STALL;
    }
      break;
    default:
      assert(0);
  }

  return alu_output;
}

uint32_t Cpu::alu_compute_op(uint32_t alu_input_a, uint32_t alu_input_b, uint32_t alu_op)
{
  uint32_t alu_output = 0xFFFFFFFF;
  switch(alu_op)
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

  return alu_output;
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

} /* namespace */
