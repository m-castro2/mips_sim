#include "cpu.h"
#include "../utils.h"
#include "../exception.h"

#include <cassert>
#include <sstream>
#include <iomanip>

using namespace std;
namespace mips_sim
{

Cpu::Cpu(shared_ptr<Memory> _memory, shared_ptr<ControlUnit> _control_unit)
  : memory(_memory), control_unit(_control_unit)
{

  /* initialize components */
  alu = unique_ptr<Alu>(new Alu(status["mult-delay"], status["div-delay"]));
  gpr_bank = shared_ptr<GPRegistersBank>(new GPRegistersBank());
  fpr_bank = shared_ptr<FPRegistersBank>(new FPRegistersBank());
  sr_bank = shared_ptr<SpecialRegistersBank>(new SpecialRegistersBank());

  loaded_instructions.reserve(400);
  loaded_instructions.push_back(0); /* start with a nop instruction */

  /* set default status */
  status["fp-add-delay"] = DEFAULT_FP_ADD_DELAY;
  status["mult-delay"]   = DEFAULT_MULT_DELAY;
  status["div-delay"]    = DEFAULT_DIV_DELAY;
  
  /* reset CPU but no memory */
  reset(false, false);
}

Cpu::~Cpu() {}

bool Cpu::is_ready( void ) const
{
  return ready;
}

void Cpu::reset( bool reset_data_memory, bool reset_text_memory )
{
  sr_bank->set(SPECIAL_PC,       MEM_TEXT_START);
  sr_bank->set(SPECIAL_HI,       0);
  sr_bank->set(SPECIAL_LO,       0);
  sr_bank->set(SPECIAL_STATUS,   0);
  sr_bank->set(SPECIAL_EPC,      0);
  sr_bank->set(SPECIAL_CAUSE,    0);
  sr_bank->set(SPECIAL_BADVADDR, 0);

  cycle = 0;
  mi_index = 0;
  execution_stall = 0;
  ready = true;

  /* set registers to 0 */
  gpr_bank->reset();
  fpr_bank->reset();

  /* stack pointer */
  gpr_bank->set("$sp", static_cast<uint32_t>(MEM_STACK_START) + MEM_STACK_SIZE);

  if (reset_data_memory)
    memory->reset(MEM_DATA_REGION);

  if (reset_text_memory)
    memory->reset(MEM_TEXT_REGION);

  loaded_instructions.clear();
  loaded_instructions.push_back(0); /* start with a nop instruction */
}

const map<string, int> Cpu::get_status() const
{
  return status;
}

bool Cpu::set_status(map<string, int> new_status, bool merge)
{
  if (!merge)
  {
    if (new_status.size() != status.size() && !merge)
      return false;
  
    status = new_status;
  }
  else
  {
    for (const auto& entry : new_status)
    {
      status[entry.first] = entry.second;
    }
  }
    
  return true;
}

void Cpu::syscall( uint32_t value )
{
  switch(value)
  {
    case 1:
      /* print_integer $a0 */
      cout << "[SYSCALL] " << gpr_bank->get("$a0") << endl;
      break;
    case 2:
      /* print_float $f12 */
      cout << "[SYSCALL] " << fpr_bank->read_float("$f12") << endl;
      break;
    case 3:
      /* print_double $f12 */
      cout << "[SYSCALL] " << fpr_bank->read_double("$f12") << endl;
      break;
    case 4:
      {
        /* print_string $a0 */
        uint32_t address = gpr_bank->get("$a0");
        uint32_t alloc_length = memory->get_allocated_length(address);
        stringstream ss;
        for (uint32_t i=0; i<alloc_length; ++i)
        {
          ss << static_cast<char>(memory->mem_read_8(address + i));
        }
        cout << "[SYSCALL] " << ss.str() << endl;
      }
      break;
    case 5:
      {
        /* read_integer -> $v0 */
        //TODO: Fails in CLI mode
        int readvalue;
        cout << "[SYSCALL] Input an integer value: ";
        cin >> readvalue;
        gpr_bank->set("$v0", static_cast<uint32_t>(readvalue));
      }
      break;
    case 6:
      {
        /* read_float -> $f0 */
        //TODO: Fails in CLI mode
        float readvalue;
        cout << "[SYSCALL] Input a float value: ";
        cin >> readvalue;
        fpr_bank->write_float("$f0", readvalue);
      }
      break;
    case 7:
      {
        /* read_double -> $f0 */
        //TODO: Fails in CLI mode
        double readvalue;
        cout << "[SYSCALL] Input a double value: ";
        cin >> readvalue;
        fpr_bank->write_double("$f0", readvalue);
      }
      break;
    case 8:
      {
        /* read_string -> $a0 of up to $a1 chars ma*/
        //TODO: Fails in CLI mode
        string readvalue;
        uint32_t address = gpr_bank->get("$a0");
        uint32_t max_length = gpr_bank->get("$a1");
        uint32_t str_len;

        cout << "[SYSCALL] Input a string value [max_length=" << max_length << "]: ";
        cin >> readvalue;
        str_len = static_cast<uint32_t>(readvalue.length());

        if (str_len > max_length)
          str_len = max_length;

        for (uint32_t i=0; i<str_len; i++)
        {
          memory->mem_write_8(address + i, static_cast<uint8_t>(readvalue[i]));
        }

        //gpr[Utils::find_register_by_name("$a1")] = static_cast<uint32_t>(str_len);
      }
      break;
    case 9:
      {
        /* allocate $a0 Bytes in data memory. Returns address in $v0 */
        uint32_t block_size = gpr_bank->get("$a0");
        uint32_t address;

        /* align block size with memory */
        block_size = memory->align_address(block_size);

        address = memory->allocate_space(block_size);

        cout << "[SYSCALL] " << block_size << " Bytes allocated at " << Utils::hex32(address) << endl;

        gpr_bank->set("$v0", address);
      }
      break;
    case 10:
      //TODO: Send stop signal or something
      /* exit program */
      cout << "[SYSCALL] Program done." << endl;
      ready = false;
      break;
    case 41:
      {
        /* random_integer $a0(seed) --> $a0 */
        srand(gpr_bank->get("$a0"));
        int rvalue = rand();
        cout << "[SYSCALL] Random integer: " << rvalue << endl;
        gpr_bank->set("$a0", static_cast<uint32_t>(rvalue));
      }
      break;
    case 42:
      {
        /* random_integer < $a1, $a0(seed) --> $a0 */
        int rvalue, ulimit;
        srand(gpr_bank->get("$a0"));
        ulimit = static_cast<int>(gpr_bank->get("$a1"));
        rvalue = rand() % ulimit;
        cout << "[SYSCALL] Random integer below " << ulimit << ": " << rvalue << endl;
        gpr_bank->set("$a0", static_cast<uint32_t>(rvalue));
      }
      break;
    case 43:
      {
        /* random_float $a0(seed) --> $f0 */
        srand(gpr_bank->get("$a0"));
        float rvalue = rand() / 0x7FFFFFFF;
        cout << "[SYSCALL] Random float: " << rvalue << endl;

        fpr_bank->write_float("$f0", rvalue);
      }
      break;
    case 44:
      {
        /* random_double $a0(seed) --> $f0 */
        srand(gpr_bank->get("$a0"));
        double rvalue = rand() / 0x7FFFFFFF;
        cout << "[SYSCALL] Random float: " << rvalue << endl;

        fpr_bank->write_double("$f0", rvalue);
      }
      break;
    default:
      throw Exception::e(CPU_SYSCALL_EXCEPTION, "Undefined syscall", value);
  }
}

void Cpu::syscall_throw_exception( uint32_t value )
{
  string message {};
  switch(value)
  {
    case 1:
      /* print_integer $a0 */
      cout << "[SYSCALL] " << gpr_bank->get("$a0") << endl;
      message = "Print $a0";
      value = gpr_bank->get("$a0");
      break;
    case 2:
      /* print_float $f12 */
      cout << "[SYSCALL] " << fpr_bank->read_float("$f12") << endl;
      message = "Print $f12";
      value = gpr_bank->read_float("$f12");
      break;
    case 3:
      /* print_double $f12 */
      cout << "[SYSCALL] " << fpr_bank->read_double("$f12") << endl;
      message = "Print $a0";
      value = gpr_bank->read_double("$f12");
      break;
    case 4:
      {
        /* print_string $a0 */
        uint32_t address = gpr_bank->get("$a0");
        uint32_t alloc_length = memory->get_allocated_length(address);
        stringstream ss;
        for (uint32_t i=0; i<alloc_length; ++i)
        {
          ss << static_cast<char>(memory->mem_read_8(address + i));
        }
        cout << "[SYSCALL] " << ss.str() << endl;
        message = "Print string $a0: ";
        message.append(ss.str());
      }
      break;
    case 5:
      {
        /* read_integer -> $v0 */
        //TODO: Fails in CLI mode
        int readvalue;
        cout << "[SYSCALL] Input an integer value: ";
        cin >> readvalue;
        gpr_bank->set("$v0", static_cast<uint32_t>(readvalue));
      }
      break;
    case 6:
      {
        /* read_float -> $f0 */
        //TODO: Fails in CLI mode
        float readvalue;
        cout << "[SYSCALL] Input a float value: ";
        cin >> readvalue;
        fpr_bank->write_float("$f0", readvalue);
      }
      break;
    case 7:
      {
        /* read_double -> $f0 */
        //TODO: Fails in CLI mode
        double readvalue;
        cout << "[SYSCALL] Input a double value: ";
        cin >> readvalue;
        fpr_bank->write_double("$f0", readvalue);
      }
      break;
    case 8:
      {
        /* read_string -> $a0 of up to $a1 chars ma*/
        //TODO: Fails in CLI mode
        string readvalue;
        uint32_t address = gpr_bank->get("$a0");
        uint32_t max_length = gpr_bank->get("$a1");
        uint32_t str_len;

        cout << "[SYSCALL] Input a string value [max_length=" << max_length << "]: ";
        cin >> readvalue;
        str_len = static_cast<uint32_t>(readvalue.length());

        if (str_len > max_length)
          str_len = max_length;

        for (uint32_t i=0; i<str_len; i++)
        {
          memory->mem_write_8(address + i, static_cast<uint8_t>(readvalue[i]));
        }

        //gpr[Utils::find_register_by_name("$a1")] = static_cast<uint32_t>(str_len);
      }
      break;
    case 9:
      {
        /* allocate $a0 Bytes in data memory. Returns address in $v0 */
        uint32_t block_size = gpr_bank->get("$a0");
        uint32_t address;

        /* align block size with memory */
        block_size = memory->align_address(block_size);

        address = memory->allocate_space(block_size);

        cout << "[SYSCALL] " << block_size << " Bytes allocated at " << Utils::hex32(address) << endl;

        gpr_bank->set("$v0", address);
      }
      break;
    case 10:
      //TODO: Send stop signal or something
      /* exit program */
      cout << "[SYSCALL] Program done." << endl;
      ready = false;
      message = "Program done";
      value = 1;
      break;
    case 41:
      {
        /* random_integer $a0(seed) --> $a0 */
        srand(gpr_bank->get("$a0"));
        int rvalue = rand();
        cout << "[SYSCALL] Random integer: " << rvalue << endl;
        gpr_bank->set("$a0", static_cast<uint32_t>(rvalue));
        message = "Random integer";
        value = rvalue;
      }
      break;
    case 42:
      {
        /* random_integer < $a1, $a0(seed) --> $a0 */
        int rvalue, ulimit;
        srand(gpr_bank->get("$a0"));
        ulimit = static_cast<int>(gpr_bank->get("$a1"));
        rvalue = rand() % ulimit;
        cout << "[SYSCALL] Random integer below " << ulimit << ": " << rvalue << endl;
        gpr_bank->set("$a0", static_cast<uint32_t>(rvalue));
        message = "Random integer below " + ulimit;
        value = rvalue;
      }
      break;
    case 43:
      {
        /* random_float $a0(seed) --> $f0 */
        srand(gpr_bank->get("$a0"));
        float rvalue = rand() / 0x7FFFFFFF;
        cout << "[SYSCALL] Random float: " << rvalue << endl;

        fpr_bank->write_float("$f0", rvalue);
        message = "Random float";
        value = rvalue;
      }
      break;
    case 44:
      {
        /* random_double $a0(seed) --> $f0 */
        srand(gpr_bank->get("$a0"));
        double rvalue = rand() / 0x7FFFFFFF;
        cout << "[SYSCALL] Random float: " << rvalue << endl;

        fpr_bank->write_double("$f0", rvalue);
        message = "Random double";
        value = rvalue;
      }
      break;
    default:
      throw Exception::e(CPU_SYSCALL_EXCEPTION, "Undefined syscall", value);
  }
  throw Exception::e(CPU_SYSCALL_EXCEPTION, message, value);
}

string Cpu::register_str(uint8_t reg_id, bool fp, bool show_value, bool show_double) const
{
  string regname;
  uint32_t regvalue[2];

  regname = fp?registers_def[reg_id].regname_fp:registers_def[reg_id].regname_int;
  regvalue[0] = fp?fpr_bank->at(reg_id):gpr_bank->at(reg_id);
  if (show_double)
    regvalue[1] = fpr_bank->at(reg_id+1);

  stringstream ss;
  ss << setw(4) << setfill(' ') << regname;
  if (show_value && !fp)
    ss << setw(10) << static_cast<int>(regvalue[0]);
  ss << " [" << Utils::hex32(regvalue[0]) << "]";
  if (show_value && fp)
  {
    ss << setw(9) << scientific << setprecision(2) << Utils::word_to_float(regvalue);
    if (show_double)
      ss << " / " << setw(9) << scientific << setprecision(2) << Utils::word_to_double(regvalue);
    else
      ss << setw(12) << " ";
  }

  return ss.str();
}

void Cpu::print_registers( ostream &out ) const
{
  out << setw(134) << setfill('-') << " " << endl;
  for (uint8_t i=0; i<16; ++i)
  {
    out << "|" << register_str(i, false, true, false);
    out << " |" << register_str(i+16, false, true, false);
    out << " | " << register_str(i, true, true, !(i%2));
    out << " | " << register_str(i+16, true, true, !(i%2));
    out << " |" << endl;
  }
  out << setw(134) << setfill('-') << " " << endl;
}

void Cpu::print_int_registers( ostream &out ) const
{
  out << setw(110) << setfill('-') << " " << endl;
  for (uint8_t i=0; i<8; ++i)
  {
    out << "|" << register_str(i, false, true, false);
    out << " |" << register_str(i+8, false, true, false);
    out << " |" << register_str(i+16, false, true, false);
    out << " |" << register_str(i+24, false, true, false);
    out << " |" << endl;
  }
  out << setw(110) << setfill('-') << " " << endl;
}

void Cpu::print_fp_registers( ostream &out ) const
{
  out << setw(80) << setfill('-') << " " << endl;
  for (uint8_t i=0; i<16; ++i)
  {
    out << "| " << register_str(i, true, true, !(i%2));
    out << " | " << register_str(i+8, true, true, !(i%2));
    out << " |" << endl;
  }
  out << setw(80) << setfill('-') << " " << endl;
}

uint32_t Cpu::read_register( uint8_t reg_index) const
{
  assert(reg_index < 32);

  return gpr_bank->at(reg_index);
}

uint32_t Cpu::read_fp_register( uint8_t reg_index) const
{
  assert(reg_index < 32);

  return fpr_bank->at(reg_index);
}

void Cpu::write_register( uint8_t reg_index, uint32_t value)
{
  if (reg_index == 0)
    throw Exception::e(CPU_REG_EXCEPTION, "Cannot write register $0");
  assert(reg_index < 32);

  gpr_bank->set_at(reg_index, value);
}

void Cpu::write_fp_register( uint8_t reg_index, uint32_t value)
{
  assert(reg_index < 32);

  fpr_bank->set_at(reg_index, value);
}

float Cpu::read_register_f( uint8_t reg_index ) const
{
  assert(reg_index < 32);

  return fpr_bank->read_float_at(reg_index);
}

uint32_t Cpu::read_special_register(string reg_name) const {

  return sr_bank->get(reg_name);
}

void Cpu::write_register_f( uint8_t reg_index, float value )
{
  assert(reg_index < 32);

  fpr_bank->write_float_at(reg_index, value);
}

double Cpu::read_register_d( uint8_t reg_index ) const
{
  if (reg_index % 2 == 1)
    throw Exception::e(CPU_REG_EXCEPTION, "Invalid register for double precision");

  assert(reg_index < 32);

  return fpr_bank->read_double_at(reg_index);
}

void Cpu::write_register_d( uint8_t reg_index, double value )
{
  if (reg_index % 2 == 1)
    throw Exception::e(CPU_REG_EXCEPTION, "Invalid register for double precision");

  assert(reg_index < 32);

  fpr_bank->write_double_at(reg_index, value);
}

bool Cpu::next_cycle( ostream &out )
{
  cycle++;

  out << "------------------------------------------ cycle "
      << dec << cycle << endl;

  return true;
}

uint32_t Cpu::get_cycle( void ) const
{
  return cycle;
}

bool Cpu::run_to_cycle( uint32_t target_cycle, ostream &out)
{
  assert(target_cycle >= 0);

  ostream nullostream(nullptr);

  /* reset CPU and memory */
  reset( true, true );

  if (target_cycle > 0)
  {
    while(cycle < (target_cycle-1) && ready)
    {
      if (!next_cycle(nullostream))
        return false;
    }
    next_cycle(out);
  }
  return true;
}

void Cpu::print_diagram( ostream &out ) const
{
  out << "Diagram not implemented" << endl;
}

const vector<uint32_t> & Cpu::get_loaded_instructions()
{
  return loaded_instructions;
}

} /* namespace */
