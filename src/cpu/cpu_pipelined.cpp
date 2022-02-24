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
  }

  CpuPipelined::~CpuPipelined()
  {

  }

/******************************************************************************/

  void CpuPipelined::stage_if( void )
  {
    uint32_t instruction_code;
    string cur_instr_name;
    /* fetch instruction */
    instruction_code = memory->mem_read_32(PC);

    cur_instr_name = Utils::decode_instruction(Utils::fill_instruction(instruction_code));
    cout << "*** " << cur_instr_name << " : " << hex << instruction_code << endl;

    /* increase PC */
    if (pc_write)
    {
      PC += 4;

      instruction_name[STAGE_IF] = cur_instr_name;

      next_seg_regs[IF_ID].data[SR_INSTRUCTION] = instruction_code;
      next_seg_regs[IF_ID].data[SR_PC] = PC;
    }
  }

/******************************************************************************/

  bool CpuPipelined::detect_hazard(uint32_t reg) const
  {
    assert(HAS_HAZARD_DETECTION_UNIT);

    /* check EX/MEM register */
    if ((seg_regs[EX_MEM].data[SR_REGDEST] == reg
         && control_unit->test(seg_regs[EX_MEM].data[SR_SIGNALS], SIG_REGWRITE))
        && ((!HAS_FORWARDING_UNIT) ||
            control_unit->test(seg_regs[EX_MEM].data[SR_SIGNALS], SIG_MEMREAD)))
    {
      return true;
    }

    /* check MEM/WB register */
    if (seg_regs[MEM_WB].data[SR_REGDEST] == reg
        && control_unit->test(seg_regs[MEM_WB].data[SR_SIGNALS], SIG_REGWRITE)
        && !HAS_FORWARDING_UNIT)
    {
      return true;
    }

    /* no hazard */
    return false;
  }

  /* ID stage: Decode and branch */
  void CpuPipelined::stage_id( void )
  {
    int mi_index = -1;
    uint32_t microinstruction;
    bool stall = false;

    /* get data from previous stage */
    uint32_t instruction_code = seg_regs[IF_ID].data[SR_INSTRUCTION];

    instruction_name[STAGE_ID] = instruction_name[STAGE_IF];
    cout << "*** " << instruction_name[STAGE_ID] << endl;

    write_instruction_register(instruction_code);

    if (instruction.code == 0)
    {
      /* NOP */
      microinstruction = 0;
      instruction_name[STAGE_ID] = "nop";
    }
    else
    {
      if (instruction.opcode == OP_RTYPE)
        mi_index = 0;
      else if (instruction.opcode == OP_J)
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
      stall = !(instruction_name[STAGE_EX] == "nop" &&
                instruction_name[STAGE_MEM] == "nop");
    }

    if (HAS_HAZARD_DETECTION_UNIT)
    {
      /* check for hazards */
      if (instruction.code > 0 && instruction.funct != SUBOP_SYSCALL && instruction.opcode != OP_LUI)
      {
        stall = detect_hazard(instruction.rs);

        if ((!control_unit->test(microinstruction, SIG_ALUSRC))
            || instruction.opcode == OP_SW)
        {
          stall |= detect_hazard(instruction.rt);
        }
      }
    }
    // cout << "   Signal ALUOp: " << control_unit->test(microinstruction, SIG_ALUOP) << endl;
    // cout << "   Signal ALUSrc: " << control_unit->test(microinstruction, SIG_ALUSRC) << endl;
    // cout << "   Signal RegDest: " << control_unit->test(microinstruction, SIG_REGDST) << endl;
    // cout << "   Signal Branch: " << control_unit->test(microinstruction, SIG_BRANCH) << endl;
    // cout << "   Signal MemRead: " << control_unit->test(microinstruction, SIG_MEMREAD) << endl;
    // cout << "   Signal MemWrite: " << control_unit->test(microinstruction, SIG_MEMWRITE) << endl;
    // cout << "   Signal Mem2Reg: " << control_unit->test(microinstruction, SIG_MEM2REG) << endl;
    // cout << "   Signal RegWrite: " << control_unit->test(microinstruction, SIG_REGWRITE) << endl;

    pc_write = !stall;
    if (stall)
    {
      /* send "NOP" to next stage */
      next_seg_regs[ID_EX] = {};
    }
    else
    {
      /* send data to next stage */
      next_seg_regs[ID_EX].data[SR_SIGNALS] = microinstruction & sigmask[ID_EX];
      next_seg_regs[ID_EX].data[SR_PC]      = seg_regs[IF_ID].data[SR_PC]; // bypass PC
      next_seg_regs[ID_EX].data[SR_RSVALUE] = gpr[instruction.rs];
      next_seg_regs[ID_EX].data[SR_RTVALUE] = gpr[instruction.rt];
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
    uint32_t rs_value = seg_regs[ID_EX].data[SR_RSVALUE];
    uint32_t rt_value = seg_regs[ID_EX].data[SR_RTVALUE];
    uint32_t addr_i   = seg_regs[ID_EX].data[SR_ADDR_I];
    uint32_t rs       = seg_regs[ID_EX].data[SR_RS];
    uint32_t rt       = seg_regs[ID_EX].data[SR_RT];
    uint32_t rd       = seg_regs[ID_EX].data[SR_RD];
    uint32_t opcode   = seg_regs[ID_EX].data[SR_OPCODE];
    uint32_t funct    = seg_regs[ID_EX].data[SR_FUNCT];

    if (opcode == 0 && funct == 0 && microinstruction == 0)
    {
      instruction_name[STAGE_EX] = "nop";
    }
    else
    {
      instruction_name[STAGE_EX] = instruction_name[STAGE_ID];
    }

    cout << "*** " << instruction_name[STAGE_EX] << endl;

    // cout << "   Signal ALUOp: " << control_unit->test(microinstruction, SIG_ALUOP) << endl;
    // cout << "   Signal ALUSrc: " << control_unit->test(microinstruction, SIG_ALUSRC) << endl;
    // cout << "   Signal RegDest: " << control_unit->test(microinstruction, SIG_REGDST) << endl;
    // cout << "   Signal Branch: " << control_unit->test(microinstruction, SIG_BRANCH) << endl;
    cout << "   Opcode: " << opcode << endl;
    cout << "   Funct: " << funct << endl;

    // alu_input_a = rs_value;
    // alu_input_b = control_unit->test(microinstruction, SIG_ALUSRC)
    //               ? addr_i
    //               : rt_value;

    /* forwarding unit */
    if (HAS_FORWARDING_UNIT)
    {
      rs_value = forward_register(rs, rs_value);
      rt_value = forward_register(rt, rt_value);

      // if (control_unit->test(microinstruction, SIG_ALUSRC) == 0)
      // {
      //   alu_input_b = forward_register(rt, rt_value);
      // }
    }

    alu_input_a = rs_value;
    alu_input_b = control_unit->test(microinstruction, SIG_ALUSRC)
                  ? static_cast<uint32_t>(static_cast<int>(addr_i) << 16 >> 16)
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
    next_seg_regs[EX_MEM].data[SR_SIGNALS]   = microinstruction & sigmask[EX_MEM];
    next_seg_regs[EX_MEM].data[SR_RELBRANCH] = (addr_i << 2) + seg_regs[ID_EX].data[SR_PC];
    next_seg_regs[EX_MEM].data[SR_ALUZERO]   = (alu_output == 0);
    next_seg_regs[EX_MEM].data[SR_ALUOUTPUT] = alu_output;
    next_seg_regs[EX_MEM].data[SR_RTVALUE]   = rt_value;
    next_seg_regs[EX_MEM].data[SR_REGDEST]   = reg_dest;
  }

/******************************************************************************/

  void CpuPipelined::stage_mem( void )
  {
    uint32_t word_read = UNDEF32;

    /* get data from previous stage */
    uint32_t microinstruction = seg_regs[EX_MEM].data[SR_SIGNALS];
    uint32_t mem_addr         = seg_regs[EX_MEM].data[SR_ALUOUTPUT];
    uint32_t rt_value         = seg_regs[EX_MEM].data[SR_RTVALUE];

    instruction_name[STAGE_MEM] = instruction_name[STAGE_EX];
    cout << "*** " << instruction_name[STAGE_MEM] << endl;

    // cout << "   Signal MemRead: " << control_unit->test(microinstruction, SIG_MEMREAD) << endl;
    // cout << "   Signal MemWrite: " << control_unit->test(microinstruction, SIG_MEMWRITE) << endl;

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
    next_seg_regs[MEM_WB].data[SR_SIGNALS]   = microinstruction & sigmask[MEM_WB];
    next_seg_regs[MEM_WB].data[SR_WORDREAD]  = word_read;
    next_seg_regs[MEM_WB].data[SR_ALUOUTPUT] = mem_addr; // come from alu output
    next_seg_regs[MEM_WB].data[SR_REGDEST]   = seg_regs[EX_MEM].data[SR_REGDEST];
  }

/******************************************************************************/

  void CpuPipelined::stage_wb( void )
  {
    /* get data from previous stage */
    uint32_t regwrite_value = UNDEF32;
    uint32_t microinstruction = seg_regs[MEM_WB].data[SR_SIGNALS];
    uint32_t reg_dest         = seg_regs[MEM_WB].data[SR_REGDEST];
    uint32_t mem_word_read    = seg_regs[MEM_WB].data[SR_WORDREAD];
    uint32_t alu_output       = seg_regs[MEM_WB].data[SR_ALUOUTPUT];

    instruction_name[STAGE_WB] = instruction_name[STAGE_MEM];
    cout << "*** " << instruction_name[STAGE_WB] << endl;

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
      cout << "   REG write " << reg_dest << " <-- " << hex << regwrite_value << endl;
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

    return ready;
  }

} /* namespace */
