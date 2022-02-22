#include "cpu.h"
#include "../utils.h"

#include <cassert>
#include <iostream>
#include <iomanip>

using namespace std;
namespace mips_sim
{

Cpu::Cpu(shared_ptr<ControlUnit> _control_unit, shared_ptr<Memory> _memory)
  : control_unit(_control_unit), memory(_memory)
{
  PC = MEM_TEXT_START;

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
      cout << "[SYSCALL] " << gpr[Utils::find_register_by_name("$a0")] << endl;
      break;
    case 2:
      //TODO: print_float $f12
      cout << "[SYSCALL] " << Utils::word_to_float<float>(&fpr[12]) << endl;
      break;
    case 3:
      //TODO: print_double $f12
      cout << "[SYSCALL] " << Utils::word_to_float<double>(&fpr[12]) << endl;
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
      cout << "Program done: exiting" << endl;
      exit(0);
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
      cout << "Undefined syscall" << endl;
      assert(0);
  }
}

void Cpu::write_instruction_register( uint32_t instruction_code )
{
  instruction.code = instruction_code;
  instruction.opcode = instruction.code >> 26;

  instruction.fp_op = (instruction.opcode == OP_FTYPE);
  if (instruction.fp_op)
  {
    /* F format fields */
    instruction.cop = (instruction.code >> 21) & 0x1F;
    instruction.rs  = (instruction.code >> 16) & 0x1F;
    instruction.rt  = (instruction.code >> 11) & 0x1F;
    instruction.rd  = (instruction.code >> 6) & 0x1F;
  }
  else
  {
    /* R format fields */
    instruction.rs = (instruction.code >> 21) & 0x1F;
    instruction.rt = (instruction.code >> 16) & 0x1F;
    instruction.rd = (instruction.code >> 11) & 0x1F;
    instruction.shamt = (instruction.code >> 6) & 0x1F;
  }
  instruction.funct = instruction.code & 0x3F;
  /* I format fields */
  instruction.addr_i = instruction.code & 0xFFFF;
  /* J format fields */
  instruction.addr_j = instruction.code & 0x3FFFFFF;

  //cout << "Instruction: " << " " << Utils::decode_instruction(instruction) << endl;
  //TMP: TEST ENCODER
  cout << "Instruction: " << " " << setw(8) << instruction.code << " " << setw(8) << Utils::encode_instruction(instruction) << " " << Utils::decode_instruction(instruction) << endl;
  cout << "Instruction asm: " << " " << setw(8) << Utils::assemble_instruction(Utils::decode_instruction(instruction)) << endl;
  cout << "Binary: 0x" << setw(8) <<  setfill('0') << hex << instruction.code << endl;
  cout << "IR write:" << hex << setfill('0')
            << " OP=0x" << setw(4)<< static_cast<uint32_t>(instruction.opcode)
            << " Rs=0x" << setw(2)<< static_cast<uint32_t>(instruction.rs)
            << " Rt=0x" << setw(2) << static_cast<uint32_t>(instruction.rt)
            << " Rd=0x" << setw(2) << static_cast<uint32_t>(instruction.rd)
            << " Shamt=0x" << setw(2) << static_cast<uint32_t>(instruction.shamt)
            << " Func=0x" << setw(2) << static_cast<uint32_t>(instruction.funct)
            << endl << "         "
            << " addr16=0x" << setw(4) << static_cast<uint32_t>(instruction.addr_i)
            << " addr26=0x" << setw(7) << static_cast<uint32_t>(instruction.addr_j)
            << endl;
}

} /* namespace */
