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
  memory->snapshot(MEM_TEXT_REGION);

  /* reset CPU but no memory */
  reset(false);
}

Cpu::~Cpu() {}

bool Cpu::is_ready( void ) const
{
  return ready;
}

void Cpu::reset( bool reset_memory )
{
  PC = MEM_TEXT_START;

  cycle = 0;
  mi_index = 0;
  execution_stall = 0;
  ready = true;

  /* set registers to 0 */
  memset(gpr, 0, 32 * sizeof(int));
  memset(fpr, 0, 32 * sizeof(int));

  if (reset_memory)
    memory->reset(MEM_TEXT_REGION);
}

uint32_t Cpu::alu_compute_subop(uint32_t alu_input_a, uint32_t alu_input_b,
                                uint8_t shift_amount, uint32_t alu_op)
{
  uint32_t alu_output = 0xFFFFFFFF;

  switch(alu_op)
  {
    case SUBOP_SYSCALL:
      /* Syscall */
      syscall(gpr[2]); // call with reg $v0
      break;
    case SUBOP_SLL:
      alu_output = alu_input_b << shift_amount; break;
    case SUBOP_SRL:
      alu_output = alu_input_b >> shift_amount; break;
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
      uint64_t v = static_cast<uint64_t>(static_cast<int32_t>(alu_input_a) *
                                         static_cast<int32_t>(alu_input_b));
      //TODO: Update HI/LO after stall
      HI = (v >> 32) & 0xFFFFFFFF;
      LO = v & 0xFFFFFFFF;
      execution_stall = MULT_DELAY;
    }
      break;
    case SUBOP_MULTU:
    {
      uint64_t v = static_cast<uint64_t>(alu_input_a * alu_input_b);
      //TODO: Update HI/LO after stall
      HI = (v >> 32) & 0xFFFFFFFF;
      LO = v & 0xFFFFFFFF;
      execution_stall = MULT_DELAY;
    }
      break;
    case SUBOP_DIV:
    {
      //TODO: Update HI/LO after stall
      /* HI = div, LO = mod */
      HI = static_cast<uint32_t>(static_cast<int32_t>(alu_input_a) / static_cast<int32_t>(alu_input_b));
      LO = static_cast<uint32_t>(static_cast<int32_t>(alu_input_a) % static_cast<int32_t>(alu_input_b));
      execution_stall = DIV_DELAY;
    }
      break;
    case SUBOP_DIVU:
    {
      //TODO: Update HI/LO after stall
      /* HI = div, LO = mod */
      HI = alu_input_a / alu_input_b;
      LO = alu_input_a % alu_input_b;
      execution_stall = DIV_DELAY;
    }
      break;
    default:
      assert(0);
  }

  return alu_output;
}

uint32_t Cpu::alu_compute_op(uint32_t alu_input_a, uint32_t alu_input_b, uint32_t alu_op) const
{
  uint32_t alu_output = 0xFFFFFFFF;
  switch(alu_op)
  {
    case OP_ADDI:
      //cout << "---[ALU] ADDI" << endl;
      alu_output = static_cast<uint32_t>(static_cast<int>(alu_input_a) + static_cast<int>(alu_input_b));
      break;
    case OP_ADDIU:
      //cout << "---[ALU] ADDIU" << endl;
      alu_output = alu_input_a + alu_input_b;
      break;
    case OP_SLTI:
      //cout << "---[ALU] SLTI" << endl;
      alu_output = (static_cast<int>(alu_input_a) < static_cast<int>(alu_input_b))?1:0;
      break;
    case OP_SLTIU:
      //cout << "---[ALU] SLTIU" << endl;
      alu_output = (alu_input_a < alu_input_b)?1:0;
      break;
    case OP_ANDI:
      //cout << "---[ALU] ANDI" << endl;
      alu_output = alu_input_a & alu_input_b;
      break;
    case OP_ORI:
      //cout << "---[ALU] ORI" << endl;
      alu_output = alu_input_a | alu_input_b;
      break;
    case OP_XORI:
      //cout << "---[ALU] XORI" << endl;
      alu_output = alu_input_a ^ alu_input_b;
      break;
    case OP_LUI:
      //cout << "---[ALU] LUI" << endl;
      alu_output = alu_input_b<<16;
      break;
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
      cout << "[SYSCALL] " << Utils::word_to_float(&fpr[12]) << endl;
      break;
    case 3:
      //TODO: print_double $f12
      cout << "[SYSCALL] " << Utils::word_to_double(&fpr[12]) << endl;
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
      cout << "[SYSCALL] Program done." << endl;
      ready = false;
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
      cout << "Undefined syscall " << value << endl;
      assert(0);
  }
}

void Cpu::print_registers( void ) const
{
  cout << endl;
  cout << setw(110) << setfill('-') << " " << endl;
  for (size_t i=0; i<8; ++i)
  {
    cout << "|" << setw(4) << setfill(' ') << registers_def[i].regname_int
         << setw(10) << static_cast<int>(gpr[i])
         << " [" << Utils::hex32(gpr[i]) << "] ";
    cout << "|" << setw(4) << setfill(' ') << registers_def[i+8].regname_int
         << setw(10) << static_cast<int>(gpr[i+8])
         << " [" << Utils::hex32(gpr[i+8]) << "] ";
    cout << "|" << setw(4) << setfill(' ') << registers_def[i+16].regname_int
         << setw(10) << static_cast<int>(gpr[i+16])
         << " [" << Utils::hex32(gpr[i+16]) << "] ";
    cout << "|" << setw(4) << setfill(' ') << registers_def[i+24].regname_int
         << setw(10) << static_cast<int>(gpr[i+24])
         << " [" << Utils::hex32(gpr[i+24]) << "] |" << endl;
  }
  cout << setw(110) << setfill('-') << " " << endl;
}

void Cpu::print_fp_registers( void ) const
{
  cout << endl;
  cout << setw(78) << setfill('-') << " " << endl;
  for (size_t i=0; i<16; ++i)
  {
    cout << "| " << setw(4) << setfill(' ') << registers_def[i].regname_fp
         << " [" << Utils::hex32(fpr[i]) << "] "
         << scientific << setprecision(2) << Utils::word_to_float(&fpr[i]);
    if (!(i%2))
      cout << " / " << scientific << setprecision(2) << Utils::word_to_double(&fpr[i]);
    else
      cout << setw(11) << " ";
    cout << " | " << setw(4) << setfill(' ') <<registers_def[i+8].regname_fp
         << " [" << Utils::hex32(fpr[i+8]) << "] "
         << scientific << setprecision(2) << Utils::word_to_float(&fpr[i+8]);
   if (!(i%2))
     cout << " / " << scientific << setprecision(2) << Utils::word_to_double(&fpr[i+8]);
   else
     cout << setw(11) << " ";
   cout << " |" << endl;
  }
  cout << setw(78) << setfill('-') << " " << endl;
}

uint32_t Cpu::read_register( size_t reg_index) const
{
  assert(reg_index < 32);

  return gpr[reg_index];
}

uint32_t Cpu::read_fp_register( size_t reg_index) const
{
  assert(reg_index < 32);

  return fpr[reg_index];
}

void Cpu::write_register( size_t reg_index, uint32_t value)
{
  assert(reg_index != 0); // cannot write $0
  assert(reg_index < 32);

  gpr[reg_index] = value;
}

void Cpu::write_fp_register( size_t reg_index, uint32_t value)
{
  assert(reg_index < 32);

  fpr[reg_index] = value;
}

float Cpu::read_register_f( size_t reg_index ) const
{
  assert(reg_index < 32);

  return Utils::word_to_float(&fpr[reg_index]);
}

void Cpu::write_register_f( size_t reg_index, float value )
{
  assert(reg_index < 32);

  Utils::float_to_word(value, &fpr[reg_index]);
}

double Cpu::read_register_d( size_t reg_index ) const
{
  assert(reg_index < 32);

  return Utils::word_to_double(&fpr[reg_index]);
}

void Cpu::write_register_d( size_t reg_index, double value )
{
  assert(reg_index < 32);
  assert(reg_index % 2 == 0);

  Utils::double_to_word(value, &fpr[reg_index]);
}

bool Cpu::next_cycle( bool verbose )
{
  cycle++;

  if (verbose)
    cout << "------------------------------------------ cycle "
         << dec << cycle << endl;
  return true;
}

bool Cpu::run_to_cycle( uint32_t target_cycle )
{
  assert(target_cycle > 0);

  /* reset CPU and memory text region */
  reset( true );
  while(cycle < target_cycle)
  {
    if (!next_cycle())
      return false;
  }

  return true;
}

} /* namespace */
