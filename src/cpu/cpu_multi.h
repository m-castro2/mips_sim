#ifndef MIPS_SIM_CPU_MULTI_H
#define MIPS_SIM_CPU_MULTI_H

#include "cpu.h"

namespace mips_sim
{

class CpuMulti : public Cpu
{
  public:

    static constexpr uint32_t uc_signal_bits[SIGNAL_COUNT] =
     { 1, 2, 1, 1, 1, 1, 2, 1, 2, 1, 1, 2, 0, 2, 1, 4 };

    static constexpr int uc_microcode_matrix[][SIGNAL_COUNT] =
    // P  P  I  M  M  I  M  R  R  R  S  S  A  A  B  C
    // C  C  o  R  W  W  2  B  D  W  A  A  S  O  r  t
    // w  s  D  d  r  r  R  k  s  r  A  B  r  p  a  d
    // 1, 2, 1, 1, 1, 1, 2, 1, 2, 1, 1, 2, -, 2, 1, 4
     {{1, 0, 0, 1, X, 1, X, 0, X, X, 0, 1, X, 0, 0, 3}, //  0 common
      {X, X, X, X, X, X, X, 0, X, X, 0, 3, X, 0, X, 1}, //  1
      {X, X, X, X, X, X, X, 0, X, X, 1, 2, X, 0, X, 2}, //  2 Mem
      {X, X, 1, 1, X, X, X, 0, X, X, X, X, X, X, X, 3}, //  3 Lw2
      {X, X, X, X, X, X, 1, 0, 0, 1, X, X, X, X, X, 0}, //  4
      {X, X, 1, X, 1, X, X, 0, X, X, X, X, X, X, X, 0}, //  5 Sw2
      {X, X, X, X, X, X, X, 0, X, X, 1, 0, X, 2, X, 4}, //  6 Rformat2
      {X, X, X, X, X, X, 0, 0, 1, 1, X, X, X, X, X, 0}, //  7 Arit3
      {1, 1, X, X, X, X, X, 0, X, X, 1, 0, X, 1, 1, 0}, //  8 Beq
      {1, 2, X, X, X, X, X, 0, X, X, X, X, X, X, X, 0}, //  9 Jump1
      {X, X, X, X, X, X, X, 0, X, X, 1, 2, X, 2, X, 3}, // 10 Iformat2
      {X, X, X, X, X, X, 0, 0, 0, 1, X, X, X, X, X, 0}, // 11
      {X, X, 1, 1, X, X, X, 0, X, X, X, X, X, X, X, 3}, // 12 FpLwc2
      {X, X, X, X, X, X, 1, 1, 0, 1, X, X, X, X, X, 0}, // 13
      {X, X, 1, X, 1, X, X, 1, X, X, X, X, X, X, X, 0}, // 14 FpSw2
      {1, 2, X, X, X, X, 2, 0, 2, 1, X, X, X, X, X, 0}, // 15 Jal
      {1, 3, X, X, X, X, X, 0, X, X, X, X, X, X, X, 0}, // 16 Jr
      {1, 3, X, X, X, X, 2, 0, 2, 1, X, X, X, X, X, 0}, // 17 Jalr
      {X, X, X, X, X, X, X, 0, X, X, 1, 0, X, 3, X, 4}, // 18 Fformat2
      {0} // end
    };

    static constexpr ctrl_dir_t uc_ctrl_dir[] =
    {
    /* OPCODE    SUBOP         JUMP1    JUMP2    JUMP4 */
      {OP_RTYPE, SUBOP_JR,     16,      UNDEF32,  7},
      {OP_RTYPE, SUBOP_JALR,   17,      UNDEF32,  7},
      {OP_RTYPE, UNDEF8,        6,      UNDEF32,  7},
      {OP_FTYPE, UNDEF8,       18,      UNDEF32,  7},
      {OP_J,     UNDEF8,        9,      UNDEF32, UNDEF32},
      {OP_JAL,   UNDEF8,       15,      UNDEF32, UNDEF32},
      {OP_BEQ,   UNDEF8,        8,      UNDEF32, UNDEF32},
      {OP_BNE,   UNDEF8,        8,      UNDEF32, UNDEF32},
      {OP_BLEZ,  UNDEF8,       UNDEF32, UNDEF32, UNDEF32},
      {OP_BGTZ,  UNDEF8,       UNDEF32, UNDEF32, UNDEF32},
      {OP_ADDI,  UNDEF8,       10,      UNDEF32,  7},
      {OP_ADDIU, UNDEF8,       10,      UNDEF32,  7},
      {OP_SLTI,  UNDEF8,       10,      UNDEF32,  7},
      {OP_SLTIU, UNDEF8,       10,      UNDEF32,  7},
      {OP_ANDI,  UNDEF8,       10,      UNDEF32,  7},
      {OP_ORI,   UNDEF8,       10,      UNDEF32,  7},
      {OP_XORI,  UNDEF8,       10,      UNDEF32,  7},
      {OP_LUI,   UNDEF8,       10,      UNDEF32,  7},
      {OP_LB,    UNDEF8,       UNDEF32, UNDEF32, UNDEF32},
      {OP_LH,    UNDEF8,       UNDEF32, UNDEF32, UNDEF32},
      {OP_LW,    UNDEF8,        2,       3,      UNDEF32},
      {OP_LBU,   UNDEF8,       UNDEF32, UNDEF32, UNDEF32},
      {OP_LHU,   UNDEF8,       UNDEF32, UNDEF32, UNDEF32},
      {OP_SB,    UNDEF8,       UNDEF32, UNDEF32, UNDEF32},
      {OP_SH,    UNDEF8,       UNDEF32, UNDEF32, UNDEF32},
      {OP_SW,    UNDEF8,        2,        5,     UNDEF32},
      {OP_LWC1,  UNDEF8,        2,       12,     UNDEF32},
      {OP_SWC1,  UNDEF8,        2,       14,     UNDEF32},
      {UNDEF8,   UNDEF8,       UNDEF32, UNDEF32, UNDEF32}
    };

    CpuMulti(std::shared_ptr<Memory>);
    CpuMulti(std::shared_ptr<ControlUnit>, std::shared_ptr<Memory>);
    virtual ~CpuMulti() override;

    virtual bool next_cycle( std::ostream &out = std::cout ) override;

  private:

    void write_instruction_register( uint32_t instruction_code,
                                     std::ostream &out = std::cout );

    /* datapath registers */
    uint32_t A_REG;
    uint32_t B_REG;
    uint32_t FA_REG[2];
    uint32_t FB_REG[2];
    uint32_t ALU_OUT_REG[2];
    uint32_t MEM_DATA_REG;
    instruction_t instruction;
};

} /* namespace */
#endif
