#include "cpu_multi.h"
#include <cassert>
#include <iostream>
#include <iomanip>
#include "../utils.h"

namespace mips_sim
{

  CpuMulti::CpuMulti(std::shared_ptr<ControlUnit> _control_unit, std::shared_ptr<Memory> _memory)
    : Cpu(_control_unit, _memory)
  {

  }

  CpuMulti::~CpuMulti()
  {

  }

  void CpuMulti::next_cycle( void )
  {
    /* temporary data */
    uint32_t alu_input_a, alu_input_b, alu_output = 0;
    float cop0_input_a = 0.0, cop0_input_b = 0.0, cop0_output = 0.0;
    double cop1_input_a = 0.0, cop1_input_b = 0.0, cop1_output = 0.0;

    uint32_t word_read = 0;

    if (execution_stall > 0)
    {
      execution_stall--;
      std::cout << "--stall" << std::endl;
      return;
    }

    uint32_t microinstruction = control_unit->get_microinstruction(mi_index);
    std::cout << "Next microinstruction [" << mi_index << "]: 0x"
              << std::setw(8) << std::setfill('0') << std::hex
              << microinstruction << std::endl;
    std::cout << "PC: 0x" << std::setw(8) << std::setfill('0') << PC
              << " A_REG: 0x" << A_REG
              << " B_REG: 0x" << B_REG << std::endl;

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
          std::cerr << "Undefined FP operation: " << instruction.code << std::endl;
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
        std::cerr << "Undefined ALU operation" << std::endl;
        assert(0);
      }
    }

    std::cout << "ALU: 0x" << std::hex << alu_input_a << " op 0x" << alu_input_b
              << " = 0x" << alu_output << std::endl;

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
        std::cout << "PC write: 0x" << std::hex << std::setfill('0') << std::setw(8)
                  << PC << std::endl;
      }
    }

    /* Update registers */

    if (control_unit->test(microinstruction, SIG_IRWRITE))
    {
        instruction.code = word_read;
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

        //std::cout << "Instruction: " << " " << Utils::decode_instruction(instruction) << std::endl;
//TMP: TEST ENCODER
std::cout << "Instruction: " << " " << std::setw(8) << instruction.code << " " << std::setw(8) << Utils::encode_instruction(instruction) << " " << Utils::decode_instruction(instruction) << std::endl;
std::cout << "Instruction asm: " << " " << std::setw(8) << Utils::assemble_instruction(Utils::decode_instruction(instruction)) << std::endl;
        std::cout << "Binary: 0x" << std::setw(8) <<  std::setfill('0') << std::hex << instruction.code << std::endl;
        std::cout << "IR write:" << std::hex << std::setfill('0')
                  << " OP=0x" << std::setw(4)<< static_cast<uint32_t>(instruction.opcode)
                  << " Rs=0x" << std::setw(2)<< static_cast<uint32_t>(instruction.rs)
                  << " Rt=0x" << std::setw(2) << static_cast<uint32_t>(instruction.rt)
                  << " Rd=0x" << std::setw(2) << static_cast<uint32_t>(instruction.rd)
                  << " Shamt=0x" << std::setw(2) << static_cast<uint32_t>(instruction.shamt)
                  << " Func=0x" << std::setw(2) << static_cast<uint32_t>(instruction.funct)
                  << std::endl << "         "
                  << " addr16=0x" << std::setw(4) << static_cast<uint32_t>(instruction.addr_i)
                  << " addr26=0x" << std::setw(7) << static_cast<uint32_t>(instruction.addr_j)
                  << std::endl;
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

        gpr[writereg] = writedata;

        std::cout << "Register write: Reg=0x"
                  << std::hex << std::setfill('0') << std::setw(2) << writereg
                  << ", Data=0x" << std::setw(8) << writedata << std::endl;
    }

    A_REG = gpr[instruction.rs];
    B_REG = gpr[instruction.rt];
    cop0_input_a = Utils::word_to_float<float>(&fpr[instruction.rs]);
    cop0_input_b = Utils::word_to_float<float>(&fpr[instruction.rt]);
    cop1_input_a = Utils::word_to_float<double>(&fpr[instruction.rs]);
    cop1_input_b = Utils::word_to_float<double>(&fpr[instruction.rt]);
    ALU_OUT_REG = alu_output;
    MEM_DATA_REG = word_read;

    /* update MI index */
    mi_index = control_unit->get_next_microinstruction(mi_index, instruction.opcode);
    if (mi_index < 0)
       exit(ERROR_UNSUPPORTED_OPERATION);
  }

} /* namespace */
