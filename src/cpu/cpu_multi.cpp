#include "cpu_multi.h"
#include "../utils.h"
#include "../exception.h"

#include <cassert>
#include <iostream>
#include <iomanip>

using namespace std;

namespace mips_sim
{

  /* deprecated since C++17 */
  //constexpr int CpuMulti::uc_microcode_matrix[][SIGNAL_COUNT];
  //constexpr uint32_t CpuMulti::uc_signal_bits[SIGNAL_COUNT];
  //constexpr ctrl_dir_t CpuMulti::uc_ctrl_dir[];

  CpuMulti::CpuMulti(shared_ptr<Memory> _memory, shared_ptr<ControlUnit> _control_unit)
    : Cpu(_memory,
          _control_unit?_control_unit:
            shared_ptr<ControlUnit>(
              new ControlUnit(CpuMulti::uc_signal_bits,
                              CpuMulti::uc_microcode_matrix,
                              CpuMulti::uc_ctrl_dir))
         )
  {
    A_REG = UNDEF32;
    B_REG = UNDEF32;
    FA_REG[0] = FA_REG[1] = UNDEF32;
    FB_REG[0] = FB_REG[1] = UNDEF32;
    ALU_OUT_REG[0] = ALU_OUT_REG[1] = UNDEF32;
    MEM_DATA_REG = UNDEF32;
    instruction = {};
  }

  CpuMulti::~CpuMulti()
  {

  }

  void CpuMulti::write_instruction_register( uint32_t instruction_code, ostream &out )
  {
    instruction = Utils::fill_instruction(instruction_code);

    out << "  -Instruction: 0x" << Utils::hex32(instruction.code) << "   ***   " << Utils::decode_instruction(instruction) << endl;
    out << "  -IR write:"
              << " OP=" << static_cast<uint32_t>(instruction.opcode)
              << " Rs=" << Utils::get_register_name(instruction.rs)
              << " Rt=" << Utils::get_register_name(instruction.rt)
              << " Rd=" << Utils::get_register_name(instruction.rd)
              << " Shamt=" << static_cast<uint32_t>(instruction.shamt)
              << " Func=" << static_cast<uint32_t>(instruction.funct)
              << endl << "            "
              << " addr16=0x" << Utils::hex32(static_cast<uint32_t>(instruction.addr_i), 4)
              << " addr26=0x" << Utils::hex32(static_cast<uint32_t>(instruction.addr_j), 7)
              << endl;
  }

  bool CpuMulti::next_cycle( ostream &out )
  {
    Cpu::next_cycle( out );

    /* temporary data */
    uint32_t alu_input_a, alu_input_b, alu_output = 0;
    float cop0_input_a = 0.0, cop0_input_b = 0.0, cop0_output = 0.0;
    double cop1_input_a = 0.0, cop1_input_b = 0.0, cop1_output = 0.0;

    uint32_t word_read = 0;

    uint32_t hi_reg = sr_bank->get(SPECIAL_HI),
             lo_reg = sr_bank->get(SPECIAL_LO);
    int stall_cycles = 0;
    bool hi_lo_updated = false;

    if (execution_stall > 0)
    {
      execution_stall--;
      out << "-- stall" << endl;
      return ready;
    }

    uint32_t microinstruction = control_unit->get_microinstruction(mi_index);
    out << "Next microinstruction [" << mi_index << "]: 0x"
              << Utils::hex32(microinstruction) << endl;
    if (mi_index > 0)
    {
      out << "   PC: [" << sr_bank->hex32_get(SPECIAL_PC)
                << "] A_REG: [" << Utils::hex32(A_REG)
                << "]    B_REG: [" << Utils::hex32(B_REG) << "]" << endl;
      if (instruction.fp_op)
      {
        if (instruction.cop == 0)
        {
          out << setw(26) << "FA_REG: [" << Utils::hex32(FA_REG[0]) << "] "
               << scientific << setprecision(2) << Utils::word_to_float(FA_REG) << endl;
          out << setw(26) << "FB_REG: [" << Utils::hex32(FB_REG[0]) << "] "
               << scientific << setprecision(2) << Utils::word_to_float(FB_REG) << endl;
          out << setw(26) << "ALU_OUT: [" << Utils::hex32(ALU_OUT_REG[0]) << "] "
               << scientific << setprecision(2) << Utils::word_to_float(ALU_OUT_REG) << " ";
        }
        else
        {
          out << setw(26) << "FA_REG: [" << Utils::hex32(FA_REG[0]) << " " << Utils::hex32(FA_REG[1]) << "] "
               << scientific << setprecision(2) << Utils::word_to_double(FA_REG) << endl;
          out << setw(26) << "FB_REG: [" << Utils::hex32(FB_REG[0]) << " " << Utils::hex32(FB_REG[1]) << "] "
               << scientific << setprecision(2) << Utils::word_to_double(FB_REG) << endl;
          out << setw(26) << "ALU_OUT: [" << Utils::hex32(ALU_OUT_REG[0]) << " " << Utils::hex32(ALU_OUT_REG[1])<< "] "
               << scientific << setprecision(2) << Utils::word_to_double(ALU_OUT_REG) << " ";
        }
      }
      else
        out << setw(26) << "ALU_OUT: [" << Utils::hex32(ALU_OUT_REG[0]) << "] ";
      out << "MEM_DATA: [" << Utils::hex32(MEM_DATA_REG) << "]" << endl;
      out << setw(26) << "HI: [" << Utils::hex32(sr_bank->get(SPECIAL_HI))
          << "]       LO: [" << Utils::hex32(sr_bank->get(SPECIAL_LO)) << "] " << endl;
    }
    else
    {
      loaded_instructions.push_back(sr_bank->get(SPECIAL_PC));
      icycle = 0;
    }

    diagram[loaded_instructions.size()-1][icycle++] = static_cast<uint32_t>(mi_index);

    if (control_unit->test(microinstruction, SIG_MEMREAD))
    {
      uint32_t address;
      if (control_unit->test(microinstruction, SIG_IOD) == 0)
        address = sr_bank->get(SPECIAL_PC);
      else
        address = ALU_OUT_REG[0];
      out << "  -Read memory address: 0x" << Utils::hex32(address) << endl;
      word_read = memory->mem_read_32(address);
      out << "  -Word read: 0x" << Utils::hex32(word_read) << endl;
    }
    else if (control_unit->test(microinstruction, SIG_MEMWRITE))
    {
      /* prevent writing instruction data */
      assert(control_unit->test(microinstruction, SIG_IOD) == 1);

      uint32_t address = ALU_OUT_REG[0];
      if (control_unit->test(microinstruction, SIG_REGBANK) == 1)
      {
        out << "  -Write memory address: 0x" << Utils::hex32(address)
             << " <-- " << Utils::hex32(FB_REG[0]) << endl;
        memory->mem_write_32(address, FB_REG[0]);
      }
      else
      {
        out << "  -Write memory address: 0x" << Utils::hex32(address)
             << " <-- " << Utils::hex32(B_REG) << endl;
        memory->mem_write_32(address, B_REG);
      }
    }

    /* ALU */
    if (control_unit->test(microinstruction, SIG_SELALUA) == 0)
        alu_input_a = sr_bank->get(SPECIAL_PC);
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

    try
    {
      switch (control_unit->test(microinstruction, SIG_ALUOP))
      {
        case 0:
          alu_output = alu->compute_subop(alu_input_a, alu_input_b,
                                        instruction.shamt, SUBOP_ADDU,
                                        &hi_reg, &lo_reg, &stall_cycles);
          hi_lo_updated = true;
          break;
        case 1:
          alu_output = alu->compute_subop(alu_input_a, alu_input_b,
                                        instruction.shamt, SUBOP_SUBU,
                                        &hi_reg, &lo_reg, &stall_cycles);
          hi_lo_updated = true;
          break;
        case 2:
          if (instruction.opcode == OP_RTYPE)
          {
            alu_output = alu->compute_subop(alu_input_a, alu_input_b,
                                          instruction.shamt, instruction.funct,
                                          &hi_reg, &lo_reg, &stall_cycles);
            hi_lo_updated = true;
          }
          else
          {
            alu_output = alu->compute_op(alu_input_a, alu_input_b, instruction.opcode);
          }
          break;
        case 3:
          //TODO
          cop0_input_a = Utils::word_to_float(FA_REG);
          cop0_input_b = Utils::word_to_float(FB_REG);
          cop1_input_a = Utils::word_to_double(FA_REG);
          cop1_input_b = Utils::word_to_double(FB_REG);
          switch(instruction.funct)
          {
            case SUBOP_FPADD:
              if (instruction.cop == 0)
                cop0_output = cop0_input_a + cop0_input_b;
              else if (instruction.cop == 1)
                cop1_output = cop1_input_a + cop1_input_b;
              execution_stall = status["fp-add-delay"];
              break;
            case SUBOP_FPSUB:
              if (instruction.cop == 0)
                cop0_output = cop0_input_a - cop0_input_b;
              else if (instruction.cop == 1)
                cop1_output = cop1_input_a - cop1_input_b;
              execution_stall = status["fp-add-delay"];
              break;
            case SUBOP_FPMUL:
              if (instruction.cop == 0)
                cop0_output = cop0_input_a * cop0_input_b;
              else if (instruction.cop == 1)
                cop1_output = cop1_input_a * cop1_input_b;
              execution_stall = status["mult-delay"];
              break;
            case SUBOP_FPDIV:
              if (instruction.cop == 0)
                cop0_output = cop0_input_a / cop0_input_b;
              else if (instruction.cop == 1)
                cop1_output = cop1_input_a / cop1_input_b;
              execution_stall = status["div-delay"];
              break;
            default:
              cerr << "Undefined FP operation: " << instruction.code << endl;
              assert(0);
          }
          break;
        default:
        {
          cerr << "Undefined ALU operation" << endl;
          assert(0);
        }
      }
    }
    catch(int e)
    {
      if (e == SYSCALL_EXCEPTION)
        syscall(gpr_bank->get("$v0"));
      else
        throw Exception::e(e, err_msg, err_v);
    }

    if (hi_lo_updated)
    {
      sr_bank->set(SPECIAL_HI, hi_reg);
      sr_bank->set(SPECIAL_LO, lo_reg);
      execution_stall = stall_cycles;
    }

    if (control_unit->test(microinstruction, SIG_ALUOP) == 3)
    {
      if (instruction.cop == 0)
        out << "   FP compute " << cop0_input_a << " OP "
             << cop0_input_b << " = "
             << cop0_output << endl;
      else
        out << "   DP compute " << cop1_input_a << " OP "
             << cop1_input_b << " = "
             << cop1_output << endl;
    }
    else
    {
      out << "   ALU compute 0x" << Utils::hex32(alu_input_a) << " OP 0x"
          << Utils::hex32(alu_input_b) << " = 0x"
          << Utils::hex32(alu_output) << endl;
    }

    /* Update registers */

    if (control_unit->test(microinstruction, SIG_IRWRITE))
    {
        write_instruction_register(word_read, out);
    }

    if (control_unit->test(microinstruction, SIG_REGWRITE))
    {
        uint8_t writereg; 
        uint32_t writedata;

        if (control_unit->test(microinstruction, SIG_REGDST) == 0)
          writereg = instruction.rt;
        else if (control_unit->test(microinstruction, SIG_REGDST) == 1)
          writereg = instruction.rd;
          else if (control_unit->test(microinstruction, SIG_REGDST) == 2)
            writereg = 31; /* for jal/jalr instructions */
        else
          assert(0);

        if (control_unit->test(microinstruction, SIG_MEM2REG) == 0)
          writedata = ALU_OUT_REG[0];
        else if (control_unit->test(microinstruction, SIG_MEM2REG) == 1)
          writedata = MEM_DATA_REG;
        else if (control_unit->test(microinstruction, SIG_MEM2REG) == 2)
          writedata = sr_bank->get(SPECIAL_PC);
        else
          assert(0);

        if (control_unit->test(microinstruction, SIG_REGBANK) == 1)
          write_fp_register(writereg, writedata);
        else if (instruction.fp_op)
        {
          //TODO
          if (instruction.cop == 0)
          {
            write_fp_register(writereg, ALU_OUT_REG[0]);
          }
          else if (instruction.cop == 1)
          {
            write_fp_register(writereg, ALU_OUT_REG[0]);
            write_fp_register(writereg+1, ALU_OUT_REG[1]);
          }
          else
            assert(0);
        }
        else
          write_register(writereg, writedata);

        out << "Register write: Reg=" << Utils::get_register_name(writereg)
            << ", Data=0x" << Utils::hex32(writedata) << endl;
    }

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
          sr_bank->set(SPECIAL_PC, alu_output);
          out << "PC write [ALU]: 0x" << sr_bank->hex32_get(SPECIAL_PC) << endl;
          break;
        case 1:
          sr_bank->set(SPECIAL_PC, ALU_OUT_REG[0]);
          out << "PC write [ALU_OUT]: 0x" << sr_bank->hex32_get(SPECIAL_PC) << endl;
          break;
        case 2:
        {
          uint32_t new_pc = (sr_bank->get(SPECIAL_PC) & 0xF0000000) + (instruction.addr_j << 2);
          sr_bank->set(SPECIAL_PC, new_pc);
          out << "PC write [J]: 0x" << Utils::hex32(new_pc) << endl;
        }
          break;
        case 3:
          sr_bank->set(SPECIAL_PC, A_REG);
          out << "PC write [REG]: 0x" << sr_bank->hex32_get(SPECIAL_PC) << endl;
          break;
        default:
          assert(0);
        }
      }
    }

    /* write registers */
    A_REG = read_register(instruction.rs);
    B_REG = read_register(instruction.rt);
    FA_REG[0] = read_fp_register(instruction.rs);
    if (!(instruction.rs%2))
      FA_REG[1] = read_fp_register(instruction.rs + 1);
    FB_REG[0] = read_fp_register(instruction.rt);
    if (!(instruction.rt%2))
      FB_REG[1] = read_fp_register(instruction.rt + 1);
    if (control_unit->test(microinstruction, SIG_ALUOP) == 3)
    {
      if (instruction.cop == 0)
        Utils::float_to_word(cop0_output, ALU_OUT_REG);
      else
        Utils::double_to_word(cop1_output, ALU_OUT_REG);
    }
    else
      ALU_OUT_REG[0] = alu_output;
    MEM_DATA_REG = word_read;

    /* update MI index */
    mi_index = control_unit->get_next_microinstruction_index(mi_index,
                                                             instruction.opcode,
                                                             instruction.funct);
    if (mi_index == UNDEF32)
       exit(ERROR_UNSUPPORTED_OPERATION);

    return ready;
  }

  void CpuMulti::print_diagram( ostream &out ) const
  {
    uint32_t c = 1;
    out << setw(24) << " " << "cycle | microinstruction" << endl;
    out << setw(56) << setfill('-') << " " << setfill(' ') << endl;

    for (size_t i = 1; i < loaded_instructions.size(); ++i)
    {
      uint32_t ipc = loaded_instructions[i];
      uint32_t icode = ipc?memory->mem_read_32(ipc):0;
      uint32_t iindex = (ipc - MEM_TEXT_START)/4 + 1;
      out << setw(2) << right << iindex << " " << setw(23) << left << Utils::decode_instruction(icode);
      int runstate = 0;
      out << setw(4) << right << c << " | ";
      for (size_t j = 0; j<=10 && runstate < 2; j++)
        if (diagram[i][j] > 0 || !runstate)
        {
          runstate = 1;
          out << right << setw(4) << (diagram[i][j]);
          c++;
        }
        else
        {
          if (runstate)
            runstate = 2;
        }
      out << endl;
    }
  }

  void CpuMulti::print_status( ostream &out ) const
  {
    out << "Multicycle CPU" << endl;
    out << "  Multiplication delay: " << status.at("mult-delay") << " cycles"  << endl;
    out << "  Division delay: " << status.at("div-delay") << " cycles" << endl;
    out << "  Floating Point Add delay: " << status.at("fp-add-delay") << " cycles" << endl;
  }

} /* namespace */
