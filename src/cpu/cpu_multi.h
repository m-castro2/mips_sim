#ifndef MIPS_SIM_CPU_MULTI_H
#define MIPS_SIM_CPU_MULTI_H

#include "cpu.h"

namespace mips_sim
{

class CpuMulti : public Cpu
{
  public:

    static constexpr uint32_t uc_signal_bits[SIGNAL_COUNT] =
     { 1, 2, 1,  1, 1, 1, 1,  1, 1, 1, 2, 0, 2, 1, 4 };

    static constexpr int uc_microcode_matrix[][SIGNAL_COUNT] =
    // P  P  I  M  M  I  M  R  R  S  S  A  A  B  C
    // C  C  o  R  W  W  2  D  W  A  A  S  O  r  t
    // w  s  D  d  r  r  R  s  r  A  B  r  p  a  d
    // 1, 2, 1, 1, 1, 1, 1, 1, 1, 1, 2, -, 2, 1, 4
     {{1, 0, 0, 1, X, 1, X, X, X, 0, 1, X, 0, 0, 3}, // common
      {X, X, X, X, X, X, X, X, X, 0, 3, X, 0, X, 1},
      {X, X, X, X, X, X, X, X, X, 1, 2, X, 0, X, 2}, // Mem
      {X, X, 1, 1, X, X, X, X, X, X, X, X, X, X, 3}, // Lw2
      {X, X, X, X, X, X, 1, 0, 1, X, X, X, X, X, 0},
      {X, X, 1, X, 1, X, X, X, X, X, X, X, X, X, 0}, // Sw2
      {X, X, X, X, X, X, X, X, X, 1, 0, X, 2, X, 4}, // Rformat2
      {X, X, X, X, X, X, 0, 1, 1, X, X, X, X, X, 0}, // Arit3
      {1, 1, X, X, X, X, X, X, X, 1, 0, X, 1, 1, 0}, // Beq
      {1, 2, X, X, X, X, X, X, X, X, X, X, X, X, 0}, // Jump1
      {X, X, X, X, X, X, X, X, X, 1, 2, X, 2, X, 3}, // Iformat2
      {X, X, X, X, X, X, 0, 0, 1, X, X, X, X, X, 0},
      {0} // end
    };

    static constexpr ctrl_dir_t uc_ctrl_dir[OP_COUNT] =
    {
      {OP_RTYPE,  6,      UNDEF32,  7},
      {OP_J,      9,      UNDEF32, UNDEF32},
      {OP_JAL,   UNDEF32, UNDEF32, UNDEF32},
      {OP_BEQ,    8,      UNDEF32, UNDEF32},
      {OP_BNE,    8,      UNDEF32, UNDEF32},
      {OP_BLEZ,  UNDEF32, UNDEF32, UNDEF32},
      {OP_BGTZ,  UNDEF32, UNDEF32, UNDEF32},
      {OP_ADDI,  10,      UNDEF32,  7},
      {OP_ADDIU, 10,      UNDEF32,  7},
      {OP_SLTI,  10,      UNDEF32,  7},
      {OP_SLTIU, 10,      UNDEF32,  7},
      {OP_ANDI,  10,      UNDEF32,  7},
      {OP_ORI,   10,      UNDEF32,  7},
      {OP_XORI,  10,      UNDEF32,  7},
      {OP_LUI,   10,      UNDEF32,  7},
      {OP_LB,    UNDEF32, UNDEF32, UNDEF32},
      {OP_LH,    UNDEF32, UNDEF32, UNDEF32},
      {OP_LW,     2,       3,      UNDEF32},
      {OP_LBU,   UNDEF32, UNDEF32, UNDEF32},
      {OP_LHU,   UNDEF32, UNDEF32, UNDEF32},
      {OP_SB,    UNDEF32, UNDEF32, UNDEF32},
      {OP_SH,    UNDEF32, UNDEF32, UNDEF32},
      {OP_SW,     2,       5,      UNDEF32}
    };

    CpuMulti(std::shared_ptr<Memory>);
    CpuMulti(std::shared_ptr<ControlUnit>, std::shared_ptr<Memory>);
    virtual ~CpuMulti() override;

    virtual bool next_cycle( bool verbose = true ) override;

  private:
    uint32_t A_REG;
    uint32_t B_REG;
    uint32_t ALU_OUT_REG;
    uint32_t MEM_DATA_REG;
};

} /* namespace */
#endif
