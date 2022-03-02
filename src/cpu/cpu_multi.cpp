#include "cpu_multi.h"
#include <cassert>
#include <iostream>
#include <iomanip>
#include "../utils.h"

using namespace std;

namespace mips_sim
{

  constexpr int CpuMulti::uc_microcode_matrix[][SIGNAL_COUNT];
  constexpr uint32_t CpuMulti::uc_signal_bits[SIGNAL_COUNT];
  constexpr ctrl_dir_t CpuMulti::uc_ctrl_dir[OP_COUNT];

  CpuMulti::CpuMulti(shared_ptr<Memory> _memory)
    : Cpu(shared_ptr<ControlUnit>(
            new ControlUnit(CpuMulti::uc_signal_bits,
                            CpuMulti::uc_microcode_matrix,
                            CpuMulti::uc_ctrl_dir)),
            _memory)
  {
    A_REG = UNDEF32;
    B_REG = UNDEF32;
    ALU_OUT_REG = UNDEF32;
    MEM_DATA_REG = UNDEF32;
  }

  CpuMulti::CpuMulti(shared_ptr<ControlUnit> _control_unit, shared_ptr<Memory> _memory)
    : Cpu(_control_unit, _memory)
  {
    A_REG = UNDEF32;
    B_REG = UNDEF32;
    ALU_OUT_REG = UNDEF32;
    MEM_DATA_REG = UNDEF32;
  }

  CpuMulti::~CpuMulti()
  {

  }

  bool CpuMulti::next_cycle( bool verbose )
  {
    Cpu::next_cycle( verbose );

    /* temporary data */
    uint32_t alu_input_a, alu_input_b, alu_output = 0;
    float cop0_input_a = 0.0, cop0_input_b = 0.0, cop0_output = 0.0;
    double cop1_input_a = 0.0, cop1_input_b = 0.0, cop1_output = 0.0;

    uint32_t word_read = 0;

    if (execution_stall > 0)
    {
      execution_stall--;
      cout << "--stall" << endl;
      return ready;
    }

    uint32_t microinstruction = control_unit->get_microinstruction(mi_index);
    cout << "Next microinstruction [" << mi_index << "]: 0x"
              << Utils::hex32(microinstruction) << endl;
    cout << "PC: 0x" << Utils::hex32(PC)
              << " A_REG: 0x" << Utils::hex32(A_REG)
              << " B_REG: 0x" << Utils::hex32(B_REG)
              << " ALU_OUT 0x" << Utils::hex32(ALU_OUT_REG)
              << " MEM_DATA 0x" << Utils::hex32(MEM_DATA_REG) << endl;

    //control_unit->print_microinstruction(mi_index);

    if (control_unit->test(microinstruction, SIG_MEMREAD))
    {
      uint32_t address;
      if (control_unit->test(microinstruction, SIG_IOD) == 0)
        address = PC;
      else
        address = ALU_OUT_REG;
      word_read = memory->mem_read_32(address);
    }
    else if (control_unit->test(microinstruction, SIG_MEMWRITE))
    {
      /* prevent writing instruction data */
      assert(control_unit->test(microinstruction, SIG_IOD) == 1);

      uint32_t address = ALU_OUT_REG;
      memory->mem_write_32(address, B_REG);
    }

    /* ALU */
    if (control_unit->test(microinstruction, SIG_SELALUA) == 0)
        alu_input_a = PC;
    else if (control_unit->test(microinstruction, SIG_SELALUA) == 1)
        alu_input_a = A_REG;
    else
        assert(0);

    if (control_unit->test(microinstruction, SIG_SELALUB) == 0)
        alu_input_b = B_REG;
    else if (control_unit->test(microinstruction, SIG_SELALUB) == 1)
        alu_input_b = 4;
    else if (control_unit->test(microinstruction, SIG_SELALUB) == 2)
    {
        /* 16 bit w sign extension */
        alu_input_b = static_cast<uint32_t>(static_cast<int>(instruction.addr_i) << 16 >> 16);
    }
    else if (control_unit->test(microinstruction, SIG_SELALUB) == 3)
    {
        alu_input_b = static_cast<uint32_t>(static_cast<int>(instruction.addr_i) << 16 >> 16);
        alu_input_b <<= 2;
    }
    else
        assert(0);

    if (instruction.fp_op)
    {
      //TODO
      switch(instruction.funct)
      {
        case SUBOP_FPADD:
          if (instruction.cop == 0)
            cop0_output = cop0_input_a + cop0_input_b;
          else if (instruction.cop == 0)
            cop1_output = cop1_input_a + cop1_input_b;
          break;
        case SUBOP_FPSUB:
          if (instruction.cop == 0)
            cop0_output = cop0_input_a - cop0_input_b;
          else if (instruction.cop == 0)
            cop1_output = cop1_input_a - cop1_input_b;
          break;
        case SUBOP_FPMUL:
          if (instruction.cop == 0)
            cop0_output = cop0_input_a * cop0_input_b;
          else if (instruction.cop == 0)
            cop1_output = cop1_input_a * cop1_input_b;
          break;
        case SUBOP_FPDIV:
          if (instruction.cop == 0)
            cop0_output = cop0_input_a / cop0_input_b;
          else if (instruction.cop == 0)
            cop1_output = cop1_input_a / cop1_input_b;
          break;
        default:
          cerr << "Undefined FP operation: " << instruction.code << endl;
          assert(0);
      }
    }
    else
    {
      if (control_unit->test(microinstruction, SIG_ALUOP) == 0)
        alu_output = alu_compute_subop(alu_input_a, alu_input_b, SUBOP_ADDU);
      else if (control_unit->test(microinstruction, SIG_ALUOP) == 1)
        alu_output = alu_compute_subop(alu_input_a, alu_input_b, SUBOP_SUBU);
      else if (control_unit->test(microinstruction, SIG_ALUOP) == 2)
      {
        if (instruction.opcode == OP_RTYPE)
        {
          alu_output = alu_compute_subop(alu_input_a, alu_input_b, instruction.funct);
        }
        else
        {
          alu_output = alu_compute_op(alu_input_a, alu_input_b, instruction.opcode);
        }
      }
      else
      {
        cerr << "Undefined ALU operation" << endl;
        assert(0);
      }
    }

    if (verbose) cout << "   ALU compute 0x" << Utils::hex32(alu_input_a) << " OP 0x"
                      << Utils::hex32(alu_input_b) << " = 0x"
                      << Utils::hex32(alu_output) << endl;

    if (control_unit->test(microinstruction, SIG_PCWRITE))
    {
      bool pcwrite = true;
      if (control_unit->test(microinstruction, SIG_BRANCH))
      {
        pcwrite = (instruction.opcode == OP_BNE && alu_output > 0) |
                  (instruction.opcode == OP_BEQ && alu_output == 0);
      }
      if (pcwrite)
      {
        switch (control_unit->test(microinstruction, SIG_PCSRC))
        {
        case 0:
          PC = alu_output;
          break;
        case 1:
          PC = ALU_OUT_REG;
          break;
        case 2:
          PC = (PC & 0xF0000000) + (instruction.addr_j << 2);
          break;
        default:
          assert(0);
        }
        cout << "PC write: 0x" << Utils::hex32(PC) << endl;
      }
    }

    /* Update registers */

    if (control_unit->test(microinstruction, SIG_IRWRITE))
    {
        write_instruction_register(word_read);
    }

    if (control_unit->test(microinstruction, SIG_REGWRITE))
    {
        uint32_t writereg, writedata;

        if (control_unit->test(microinstruction, SIG_REGDST) == 0)
          writereg = instruction.rt;
        else if (control_unit->test(microinstruction, SIG_REGDST) == 1)
          writereg = instruction.rd;
        else
          assert(0);

        if (control_unit->test(microinstruction, SIG_MEM2REG) == 0)
          writedata = ALU_OUT_REG;
        else if (control_unit->test(microinstruction, SIG_MEM2REG) == 1)
          writedata = MEM_DATA_REG;
        else
          assert(0);

        write_register(writereg, writedata);

        cout << "Register write: Reg=" << Utils::get_register_name(writereg)
                  << ", Data=0x" << Utils::hex32(writedata) << endl;
    }


    A_REG = read_register(instruction.rs);
    B_REG = read_register(instruction.rt);
    cout << "Set A [" << dec << static_cast<uint32_t>(instruction.rs) << "]  = 0x" << Utils::hex32(A_REG) << endl;
    cout << "Set B [" << dec << static_cast<uint32_t>(instruction.rt) << "]  = 0x" << Utils::hex32(B_REG) << endl;
    cop0_input_a = read_register_f(instruction.rs);
    cop0_input_b = read_register_f(instruction.rt);
    cop1_input_a = read_register_d(instruction.rs);
    cop1_input_b = read_register_d(instruction.rt);
    ALU_OUT_REG = alu_output;
    MEM_DATA_REG = word_read;

    /* update MI index */
    mi_index = control_unit->get_next_microinstruction(mi_index, instruction.opcode);
    if (mi_index == UNDEF32)
       exit(ERROR_UNSUPPORTED_OPERATION);

    return ready;
  }

} /* namespace */
