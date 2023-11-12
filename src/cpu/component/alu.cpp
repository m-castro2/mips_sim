#include "alu.h"
#include "../../exception.h"

#include <cassert>

namespace mips_sim
{

Alu::Alu(int _mult_delay, int _div_delay)
        : mult_delay(_mult_delay), div_delay(_div_delay)
{
}

Alu::~Alu()
{
}

uint32_t Alu::compute_op(uint32_t alu_input_a, uint32_t alu_input_b, uint32_t alu_op) const
{
  uint32_t alu_output = 0xFFFFFFFF;
  switch(alu_op)
  {
    case OP_ADDI:
      alu_output = static_cast<uint32_t>(static_cast<int>(alu_input_a) + static_cast<int>(alu_input_b));
      break;
    case OP_ADDIU:
      alu_output = alu_input_a + alu_input_b;
      break;
    case OP_SLTI:
      alu_output = (static_cast<int>(alu_input_a) < static_cast<int>(alu_input_b))?1:0;
      break;
    case OP_SLTIU:
      alu_output = (alu_input_a < alu_input_b)?1:0;
      break;
    case OP_ANDI:
      alu_output = alu_input_a & alu_input_b;
      break;
    case OP_ORI:
      alu_output = alu_input_a | alu_input_b;
      break;
    case OP_XORI:
      alu_output = alu_input_a ^ alu_input_b;
      break;
    case OP_LUI:
      alu_output = alu_input_b<<16;
      break;
    default:
      throw Exception::e(CPU_UNDEF_EXCEPTION, "Undefined ALU operation ", alu_op);
  }

  return alu_output;
}

uint32_t Alu::compute_subop(uint32_t alu_input_a, uint32_t alu_input_b,
                            uint8_t shift_amount, uint32_t alu_op,
                            uint32_t *HI, uint32_t *LO, int *execution_stall, bool exception_disabled)
{
  uint32_t alu_output = 0xFFFFFFFF;

  switch(alu_op)
  {
    case SUBOP_SYSCALL:
      /* Syscall. The processor should handle it */
      if (!exception_disabled)
        throw Exception::e(SYSCALL_EXCEPTION, "syscall");
      else
        is_syscall = true;
      break;
    case SUBOP_SLL:
      alu_output = alu_input_b << shift_amount; break;
    case SUBOP_SRL:
      /* parameters are unsigned, thus this op should work correctly */
      alu_output = alu_input_b >> shift_amount; break;
    case SUBOP_SRA:
      /* we assume arithmetic shift is the default compiler behavior */
      alu_output = static_cast<uint32_t>(static_cast<int32_t>(alu_input_b) >> shift_amount); break;
    case SUBOP_SLLV:
      alu_output = alu_input_a << alu_input_b; break;
    case SUBOP_SRLV:
      /* parameters are unsigned, thus this op should work correctly */
      alu_output = alu_input_a >> alu_input_b; break;
    case SUBOP_SRAV:
      /* we assume arithmetic shift is the default compiler behavior */
      alu_output = static_cast<uint32_t>(static_cast<int32_t>(alu_input_a) >> alu_input_b); break;
    case SUBOP_AND:
      alu_output = alu_input_a & alu_input_b; break;
    case SUBOP_OR:
      alu_output = alu_input_a | alu_input_b; break;
    case SUBOP_XOR:
      alu_output = alu_input_a ^ alu_input_b; break;
    case SUBOP_NOR:
      alu_output = ~(alu_input_a | alu_input_b); break;
    case SUBOP_JR:
    case SUBOP_JALR:
      alu_output = alu_input_a; break;
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
      alu_output = alu_input_a - alu_input_b; break;
    case SUBOP_MULT:
    {
      assert(HI && LO && execution_stall);
      uint64_t v = static_cast<uint64_t>(static_cast<int32_t>(alu_input_a) *
                                         static_cast<int32_t>(alu_input_b));

      //TODO: Update HI/LO after stall
      *HI = (v >> 32) & 0xFFFFFFFF;
      *LO = v & 0xFFFFFFFF;
      *execution_stall = mult_delay;
    }
      break;
    case SUBOP_MULTU:
    {
      assert(HI && LO && execution_stall);
      
      uint64_t v = static_cast<uint64_t>(alu_input_a * alu_input_b);
      //TODO: Update HI/LO after stall
      *HI = (v >> 32) & 0xFFFFFFFF;
      *LO = v & 0xFFFFFFFF;
      *execution_stall = mult_delay;
    }
      break;
    case SUBOP_DIV:
    {
      assert(HI && LO && execution_stall);
      
      //TODO: Update HI/LO after stall
      /* HI = div, LO = mod */
      *HI = static_cast<uint32_t>(static_cast<int32_t>(alu_input_a) / static_cast<int32_t>(alu_input_b));
      *LO = static_cast<uint32_t>(static_cast<int32_t>(alu_input_a) % static_cast<int32_t>(alu_input_b));
      *execution_stall = div_delay;
    }
      break;
    case SUBOP_DIVU:
    {
      assert(HI && LO && execution_stall);
      
      //TODO: Update HI/LO after stall
      /* HI = div, LO = mod */
      *HI = alu_input_a / alu_input_b;
      *LO = alu_input_a % alu_input_b;
      *execution_stall = div_delay;
    }
      break;
    default:
      throw Exception::e(CPU_UNDEF_EXCEPTION, "Undefined ALU operation ", alu_op);
  }

  return alu_output;
}
} /* namespace */
