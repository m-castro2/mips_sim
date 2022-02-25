#include "cpu_pipelined.h"

#include <cassert>
#include <iostream>
#include <iomanip>
#include "../utils.h"

using namespace std;

namespace mips_sim
{

  CpuPipelined::CpuPipelined(std::shared_ptr<ControlUnit> _control_unit, std::shared_ptr<Memory> _memory)
    : Cpu(_control_unit, _memory)
  {
    /* signals sorted in reverse order */
    signal_t signals_ID[] = {
      SIG_MEM2REG, SIG_REGWRITE, // WB stage
      SIG_MEMREAD, SIG_MEMWRITE, // MEM stage
      SIG_BRANCH, SIG_PCSRC, SIG_ALUSRC, SIG_ALUOP, SIG_REGDST}; // EX stage

    /* build signals bitmask as the number of signals passed to the next stage */
    sigmask[IF_ID]  = UNDEF32;
    sigmask[ID_EX]  = control_unit->get_signal_bitmask(signals_ID, 9);
    sigmask[EX_MEM] = control_unit->get_signal_bitmask(signals_ID, 6);
    sigmask[MEM_WB] = control_unit->get_signal_bitmask(signals_ID, 2);

    pc_write = true;
    flush_pipeline = 0;
    next_pc = 0;
  }

  CpuPipelined::~CpuPipelined()
  {

  }

  bool CpuPipelined::process_branch(uint32_t instruction_code,
                      uint32_t rs_value, uint32_t rt_value,
                      uint32_t pc_value)
  {
    instruction_t instruction = Utils::fill_instruction(instruction_code);

    uint32_t opcode = instruction.opcode;
    uint32_t funct = instruction.funct;
    uint32_t addr_i32 = static_cast<uint32_t>(static_cast<int>(instruction.addr_i) << 16 >> 16);

    bool conditional_branch = (BRANCH_STAGE == STAGE_ID) &&
                              ((rs_value == rt_value && opcode == OP_BEQ)
                           || (rs_value != rt_value && opcode == OP_BNE));

    bool branch_taken = conditional_branch
          || opcode == OP_J || opcode == OP_JAL
          || (opcode == OP_RTYPE && (funct == SUBOP_JR || funct == SUBOP_JALR));

    if (branch_taken)
    {
      uint32_t branch_addr = 0;
      if (opcode == OP_BEQ || opcode == OP_BNE)
      {
        branch_addr = pc_value + (addr_i32 << 2);
      }
      else if (opcode == OP_J || opcode == OP_JAL)
      {
        branch_addr = (pc_value & 0xF0000000) | (instruction.addr_j << 2);
      }
      else if (funct == SUBOP_JR || funct == SUBOP_JALR)
      {
        branch_addr = rs_value;
      }

      cout << "  BRANCH: Jump to " << hex << branch_addr << endl;

      next_pc = branch_addr;
    }

    return branch_taken;
  }

/******************************************************************************/

  void CpuPipelined::stage_if( void )
  {
    uint32_t instruction_code;
    string cur_instr_name;
    /* fetch instruction */
    instruction_code = memory->mem_read_32(PC);

    cur_instr_name = Utils::decode_instruction(Utils::fill_instruction(instruction_code));
    cout << "   *** PC: " << hex << PC << endl;
    cout << "   *** " << cur_instr_name << " : " << hex << instruction_code << endl;

    /* increase PC */
    if (pc_write)
    {
      PC += 4;

      next_seg_regs[IF_ID].data[SR_INSTRUCTION] = instruction_code;
      next_seg_regs[IF_ID].data[SR_PC] = PC;
    }
  }

/******************************************************************************/

  bool CpuPipelined::detect_hazard( uint32_t read_reg, bool can_forward ) const
  {
    assert(HAS_HAZARD_DETECTION_UNIT);

    /* check next 2 registers as forwarding would happen on next cycle */
    uint32_t ex_regdest   = seg_regs[ID_EX].data[SR_REGDEST];
    uint32_t ex_regwrite  = control_unit->test(seg_regs[ID_EX].data[SR_SIGNALS], SIG_REGWRITE);
    uint32_t ex_memread   = control_unit->test(seg_regs[ID_EX].data[SR_SIGNALS], SIG_MEMREAD);
    uint32_t mem_regdest  = seg_regs[EX_MEM].data[SR_REGDEST];
    uint32_t mem_regwrite = control_unit->test(seg_regs[EX_MEM].data[SR_SIGNALS], SIG_REGWRITE);

    bool hazard = (ex_regdest == read_reg) && ex_regwrite && (ex_memread || !can_forward);

    hazard |= (mem_regdest == read_reg) && mem_regwrite && !can_forward;

    return hazard;
  }

  /* ID stage: Decode and branch */
  void CpuPipelined::stage_id( void )
  {
    int mi_index = -1;
    uint32_t microinstruction;
    bool stall = false;
    uint32_t rs_value, rt_value;

    /* get data from previous stage */
    uint32_t instruction_code = seg_regs[IF_ID].data[SR_INSTRUCTION];
    uint32_t pc_value = seg_regs[IF_ID].data[SR_PC];

    cout << "   *** " << Utils::decode_instruction(Utils::fill_instruction(instruction_code)) << endl;

    write_instruction_register(instruction_code);

    rs_value = gpr[instruction.rs];
    rt_value = gpr[instruction.rt];

    if (instruction.code == 0)
    {
      /* NOP */
      microinstruction = 0;
    }
    else
    {
      if (instruction.opcode == OP_RTYPE)
      {
        if (instruction.funct == SUBOP_JR || instruction.funct == SUBOP_JALR)
          mi_index = 1;
        else
          mi_index = 0;
      }
      else if (instruction.opcode == OP_J || instruction.opcode == OP_JAL)
        mi_index = 1;
      else if (instruction.opcode == OP_BNE || instruction.opcode == OP_BEQ)
        mi_index = 2;
      else if (instruction.opcode == OP_LW)
        mi_index = 3;
      else if (instruction.opcode == OP_SW)
        mi_index = 4;
      else
        mi_index = 5;

      microinstruction = control_unit->get_microinstruction(mi_index);
      cout << "   Microinstruction: [" << mi_index << "]: " << hex << microinstruction << endl;
    }

    if (instruction.opcode == 0 && instruction.funct == SUBOP_SYSCALL)
    {
      /* stalls until previous instructions finished */
      stall = !(seg_regs[ID_EX].data[SR_INSTRUCTION] == 0 &&
                seg_regs[EX_MEM].data[SR_INSTRUCTION] == 0);
    }

    if (instruction.opcode == OP_JAL
      || (instruction.opcode == OP_RTYPE && instruction.funct == SUBOP_JALR))
    {
      /* hack the processor! */
      rs_value = seg_regs[IF_ID].data[SR_PC];
      rt_value = 0;
      instruction.rd = 31; // $ra
      control_unit->set(microinstruction, SIG_MEM2REG, 1);
      control_unit->set(microinstruction, SIG_REGWRITE, 1);
      control_unit->set(microinstruction, SIG_REGDST, 1);
    }

    if (HAS_HAZARD_DETECTION_UNIT)
    {
      bool can_forward = HAS_FORWARDING_UNIT &&
                         ((instruction.opcode != OP_BNE && instruction.opcode != OP_BEQ)
                           || BRANCH_STAGE > STAGE_ID);
      /* check for hazards */
      if (instruction.code > 0 && instruction.funct != SUBOP_SYSCALL && instruction.opcode != OP_LUI)
      {
        stall = detect_hazard(instruction.rs, can_forward);

        if ((!control_unit->test(microinstruction, SIG_ALUSRC))
            || instruction.opcode == OP_SW)
        {
          stall |= detect_hazard(instruction.rt, can_forward);
        }
      }

      if (stall) cout << "   Hazard detected: Pipeline stall" << endl;
    }

    pc_write = !stall;
    if (stall)
    {
      /* send "NOP" to next stage */
      next_seg_regs[ID_EX] = {};
    }
    else
    {
      if (control_unit->test(microinstruction, SIG_BRANCH))
      {
        /* unconditional branches are resolved here */
        bool branch_taken = process_branch(instruction_code,
                                           rs_value, rt_value, pc_value);

        if (BRANCH_TYPE == BRANCH_FLUSH
            || (BRANCH_TYPE == BRANCH_NON_TAKEN && branch_taken))
        {
          flush_pipeline = 1;
        }
      }

      /* send data to next stage */
      next_seg_regs[ID_EX].data[SR_INSTRUCTION] = instruction_code;
      next_seg_regs[ID_EX].data[SR_SIGNALS] = microinstruction & sigmask[ID_EX];
      next_seg_regs[ID_EX].data[SR_PC]      = pc_value; // bypass PC
      next_seg_regs[ID_EX].data[SR_RSVALUE] = rs_value;
      next_seg_regs[ID_EX].data[SR_RTVALUE] = rt_value;
      next_seg_regs[ID_EX].data[SR_ADDR_I]  = instruction.addr_i;
      next_seg_regs[ID_EX].data[SR_RT]      = instruction.rt;
      next_seg_regs[ID_EX].data[SR_RD]      = instruction.rd;
      next_seg_regs[ID_EX].data[SR_FUNCT]   = instruction.funct;
      next_seg_regs[ID_EX].data[SR_OPCODE]  = instruction.opcode;
      next_seg_regs[ID_EX].data[SR_RS]      = instruction.rs;
    }
  }

/******************************************************************************/

  uint32_t CpuPipelined::forward_register( uint32_t reg, uint32_t reg_value ) const
  {
    assert(HAS_FORWARDING_UNIT);

    uint32_t mem_regdest  = seg_regs[EX_MEM].data[SR_REGDEST];
    uint32_t mem_regvalue = seg_regs[EX_MEM].data[SR_ALUOUTPUT];
    uint32_t wb_regdest   = seg_regs[MEM_WB].data[SR_REGDEST];
    uint32_t wb_regvalue  = seg_regs[MEM_WB].data[SR_ALUOUTPUT];

    if (reg == 0)
      return reg_value;

    /* check EX/MEM register */
    if (mem_regdest == reg &&
        !control_unit->test(seg_regs[EX_MEM].data[SR_SIGNALS], SIG_MEMREAD) &&
        control_unit->test(seg_regs[EX_MEM].data[SR_SIGNALS], SIG_REGWRITE))
    {
      cout << " -- forward " << reg << " [0x" << hex << mem_regvalue << "] from EX/MEM" << endl;
      return mem_regvalue;
    }

    /* check MEM/WB register */
    if (wb_regdest == reg &&
        control_unit->test(seg_regs[MEM_WB].data[SR_SIGNALS], SIG_REGWRITE))
    {
      cout << " -- forward " << reg << " [0x" << hex << wb_regvalue << "] from MEM/WB" << endl;
      return wb_regvalue;
    }

    return reg_value;
  }

  void CpuPipelined::stage_ex( void )
  {
    uint32_t microinstruction = seg_regs[ID_EX].data[SR_SIGNALS];
    uint32_t alu_input_a, alu_input_b, alu_output;
    uint32_t reg_dest;

    /* get data from previous stage */
    uint32_t instruction_code = seg_regs[ID_EX].data[SR_INSTRUCTION];
    uint32_t rs_value = seg_regs[ID_EX].data[SR_RSVALUE];
    uint32_t rt_value = seg_regs[ID_EX].data[SR_RTVALUE];
    uint32_t addr_i   = seg_regs[ID_EX].data[SR_ADDR_I];
    uint32_t rs       = seg_regs[ID_EX].data[SR_RS];
    uint32_t rt       = seg_regs[ID_EX].data[SR_RT];
    uint32_t rd       = seg_regs[ID_EX].data[SR_RD];
    uint32_t opcode   = seg_regs[ID_EX].data[SR_OPCODE];
    uint32_t funct    = seg_regs[ID_EX].data[SR_FUNCT];

    uint32_t addr_i32 = static_cast<uint32_t>(static_cast<int>(addr_i) << 16 >> 16);

    cout << "   *** " << Utils::decode_instruction(Utils::fill_instruction(instruction_code)) << endl;

    /* forwarding unit */
    if (HAS_FORWARDING_UNIT)
    {
      rs_value = forward_register(rs, rs_value);
      rt_value = forward_register(rt, rt_value);
    }

    alu_input_a = rs_value;
    alu_input_b = control_unit->test(microinstruction, SIG_ALUSRC)
                  ? addr_i32
                  : rt_value;

    switch (control_unit->test(microinstruction, SIG_ALUOP))
    {
      case 0:
        alu_output = alu_compute_subop(alu_input_a, alu_input_b, SUBOP_ADDU);
        break;
      case 1:
        alu_output = alu_compute_subop(alu_input_a, alu_input_b, SUBOP_SUBU);
        break;
      case 2:
        if (opcode == OP_RTYPE)
          alu_output = alu_compute_subop(alu_input_a, alu_input_b, funct);
        else
          alu_output = alu_compute_op(alu_input_a, alu_input_b, opcode);
        break;
      default:
        std::cerr << "Undefined ALU operation" << std::endl;
        assert(0);
    }

    cout << "   ALU compute " << hex << alu_input_a << " OP " << hex << alu_input_b << " = " << hex << alu_output << endl;

    reg_dest = control_unit->test(microinstruction, SIG_REGDST)
                  ? rd
                  : rt;

    /* send data to next stage */
    next_seg_regs[EX_MEM].data[SR_INSTRUCTION] = instruction_code;
    next_seg_regs[EX_MEM].data[SR_SIGNALS]   = microinstruction & sigmask[EX_MEM];
    next_seg_regs[EX_MEM].data[SR_RELBRANCH] = (addr_i32 << 2) + seg_regs[ID_EX].data[SR_PC];
    next_seg_regs[EX_MEM].data[SR_ALUZERO]   = ((alu_output == 0) && (opcode == OP_BEQ))
                                               || ((alu_output != 0) && (opcode == OP_BNE));
    next_seg_regs[EX_MEM].data[SR_ALUOUTPUT] = alu_output;
    next_seg_regs[EX_MEM].data[SR_RTVALUE]   = rt_value;
    next_seg_regs[EX_MEM].data[SR_REGDEST]   = reg_dest;
  }

/******************************************************************************/

  void CpuPipelined::stage_mem( void )
  {
    uint32_t word_read = UNDEF32;

    /* get data from previous stage */
    uint32_t instruction_code = seg_regs[EX_MEM].data[SR_INSTRUCTION];
    uint32_t microinstruction = seg_regs[EX_MEM].data[SR_SIGNALS];
    uint32_t mem_addr         = seg_regs[EX_MEM].data[SR_ALUOUTPUT];
    uint32_t rt_value         = seg_regs[EX_MEM].data[SR_RTVALUE];
    uint32_t branch_addr      = seg_regs[EX_MEM].data[SR_RELBRANCH];
    uint32_t alu_zero         = seg_regs[EX_MEM].data[SR_ALUZERO];

    cout << "   *** " << Utils::decode_instruction(Utils::fill_instruction(instruction_code)) << endl;

    if (BRANCH_STAGE == STAGE_MEM && control_unit->test(microinstruction, SIG_BRANCH))
    {
      /* if conditional branches are resolved here */
      if (BRANCH_TYPE == BRANCH_FLUSH
          || (BRANCH_TYPE == BRANCH_NON_TAKEN && alu_zero))
      {
        flush_pipeline = 3;
      }

      if (alu_zero)
      {
        cout << "  BRANCH: Jump to " << hex << branch_addr << endl;

        next_pc = branch_addr;
      }
    }

    if (control_unit->test(microinstruction, SIG_MEMREAD))
    {
      cout << "   MEM read " << mem_addr;
      word_read = memory->mem_read_32(mem_addr);
    }
    else if (control_unit->test(microinstruction, SIG_MEMWRITE))
    {
      cout << "   MEM write " << rt_value << " to 0x" << hex << mem_addr << endl;
      memory->mem_write_32(mem_addr, rt_value);
    }

    // ...
    cout << "   Memory read: " << word_read << endl;
    cout << "   Address/Bypass: " << mem_addr << endl;

    /* send data to next stage */
    next_seg_regs[MEM_WB].data[SR_INSTRUCTION] = instruction_code;
    next_seg_regs[MEM_WB].data[SR_SIGNALS]   = microinstruction & sigmask[MEM_WB];
    next_seg_regs[MEM_WB].data[SR_WORDREAD]  = word_read;
    next_seg_regs[MEM_WB].data[SR_ALUOUTPUT] = mem_addr; // come from alu output
    next_seg_regs[MEM_WB].data[SR_REGDEST]   = seg_regs[EX_MEM].data[SR_REGDEST];
  }

/******************************************************************************/

  void CpuPipelined::stage_wb( void )
  {
    uint32_t regwrite_value = UNDEF32;

    /* get data from previous stage */
    uint32_t instruction_code = seg_regs[MEM_WB].data[SR_INSTRUCTION];
    uint32_t microinstruction = seg_regs[MEM_WB].data[SR_SIGNALS];
    uint32_t reg_dest         = seg_regs[MEM_WB].data[SR_REGDEST];
    uint32_t mem_word_read    = seg_regs[MEM_WB].data[SR_WORDREAD];
    uint32_t alu_output       = seg_regs[MEM_WB].data[SR_ALUOUTPUT];

    cout << "   *** " << Utils::decode_instruction(Utils::fill_instruction(instruction_code)) << endl;

    cout << "   Result value: " << hex << regwrite_value << endl;
    cout << "   Register dest: " << reg_dest << endl;
    cout << "   Signal Mem2Reg: " << control_unit->test(microinstruction, SIG_MEM2REG) << endl;
    cout << "   Signal RegWrite: " << control_unit->test(microinstruction, SIG_REGWRITE) << endl;

    if (control_unit->test(microinstruction, SIG_MEM2REG) == 0)
      regwrite_value = mem_word_read;
    else
      regwrite_value = alu_output;

    if (control_unit->test(microinstruction, SIG_REGWRITE))
    {
      cout << "   REG write " << reg_dest << " <-- 0x" << hex << regwrite_value << endl;
      gpr[reg_dest] = regwrite_value;
    }
  }

/******************************************************************************/

  bool CpuPipelined::next_cycle( void )
  {
    cout << "WB stage" << endl;
    stage_wb();
    cout << "MEM stage" << endl;
    stage_mem();
    cout << "EX stage" << endl;
    stage_ex();
    cout << "ID stage" << endl;
    stage_id();
    cout << "IF stage" << endl;
    stage_if();

    /* update segmentation registers */
    memcpy(seg_regs, next_seg_regs, sizeof(seg_regs));

    if (flush_pipeline > 0)
    {
      for (int i = 0; i < flush_pipeline; ++i)
        seg_regs[i] = {};

      flush_pipeline = 0;
    }

    if (next_pc != 0)
    {
      PC = next_pc;
      next_pc = 0;
    }

    return ready;
  }

} /* namespace */
