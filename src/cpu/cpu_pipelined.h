#ifndef MIPS_SIM_CPU_PIPELINED_H
#define MIPS_SIM_CPU_PIPELINED_H

#define STAGE_IF      0
#define STAGE_ID      1
#define STAGE_EX      2
#define STAGE_MEM     3
#define STAGE_WB      4
#define STAGE_COUNT   5

#define IF_ID  0
#define ID_EX  1
#define EX_MEM 2
#define MEM_WB 3

#define SR_SIGNALS      0
#define SR_PC           1
#define SR_RSVALUE      2
#define SR_RTVALUE      3
#define SR_ADDR_I       4
#define SR_RS           5
#define SR_RT           6
#define SR_RD           7
#define SR_FUNCT        8
#define SR_OPCODE       9
#define SR_ALUOUTPUT   10
#define SR_ALUZERO     11
#define SR_RELBRANCH   12
#define SR_REGDEST     13
#define SR_WORDREAD    14
#define SR_SHAMT       15
#define SR_INSTRUCTION 31

#define BRANCH_FLUSH     0 /* flush pipeline */
#define BRANCH_NON_TAKEN 1 /* fixed non taken */
#define BRANCH_DELAYED   2 /* delayed branch */

#include "cpu.h"

namespace mips_sim
{

const int  BRANCH_TYPE               = BRANCH_NON_TAKEN;
const int  BRANCH_STAGE              = STAGE_ID;
const bool HAS_FORWARDING_UNIT       = true;
const bool HAS_HAZARD_DETECTION_UNIT = true;

/* segmentation register */
struct seg_reg_t {
  uint32_t data[32];
};

struct op_microcode_t {
  uint8_t opcode;
  uint8_t subopcode;
  uint32_t microinstruction_index;
};

class CpuPipelined : public Cpu
{
  public:

    static constexpr uint32_t uc_signal_bits[SIGNAL_COUNT] =
     { 0, 2, 0, 1, 1, 0, 2, 1, 2, 1, 0, 0, 1, 2, 1, 0 };

    static constexpr int uc_microcode_matrix[][SIGNAL_COUNT] =
    // P  P  I  M  M  I  M  R  R  R  S  S  A  A  B  C
    // C  C  o  R  W  W  2  B  D  W  A  A  S  O  r  t
    // w  s  D  d  r  r  R  k  s  r  A  B  r  p  a  d
    // -  1  -  1  1  -  2  1  2  1  -  -  1  2  1  -
     {{X, 0, X, 0, 0, X, 1, 0, 1, 1, X, X, 0, 2, 0, X}, //  0 R type
      {X, 3, X, 0, 0, X, 0, 0, 0, 0, X, X, 0, 0, 1, X}, //  1 J
      {X, 2, X, 0, 0, X, 0, 0, 0, 0, X, X, 0, 0, 1, X}, //  2 JR
      {X, 3, X, 0, 0, X, 2, 0, 2, 1, X, X, 0, 0, 1, X}, //  3 JAL
      {X, 2, X, 0, 0, X, 2, 0, 2, 1, X, X, 0, 0, 1, X}, //  4 JALR
      {X, 1, X, 0, 0, X, 0, 0, 0, 0, X, X, 0, 1, 1, X}, //  5 BNE/BEQ
      {X, 0, X, 1, 0, X, 0, 0, 0, 1, X, X, 1, 0, 0, X}, //  6 LW
      {X, 0, X, 0, 1, X, 0, 0, 0, 0, X, X, 1, 0, 0, X}, //  7 SW
      {X, 0, X, 0, 0, X, 1, 0, 0, 1, X, X, 1, 2, 0, X}, //  8 I type
      {X, 0, X, 0, 0, X, 1, 1, 1, 1, X, X, 0, 2, 0, X}, //  9 FType add/sub/mul/div
      {X, 0, X, 1, 0, X, 0, 1, 0, 1, X, X, 1, 0, 0, X}, // 10 LWC1
      {0} // end
    };

    static constexpr op_microcode_t op_select[] =
    {
      {OP_RTYPE, SUBOP_JR,    2},
      {OP_RTYPE, SUBOP_JALR,  4},
      {OP_RTYPE, UNDEF8,      0},
      {OP_J,     UNDEF8,      1},
      {OP_JAL,   UNDEF8,      3},
      {OP_BNE,   UNDEF8,      5},
      {OP_BEQ,   UNDEF8,      5},
      {OP_LW,    UNDEF8,      6},
      {OP_LWC1,  UNDEF8,     10},
      {OP_SW,    UNDEF8,      7},
      {OP_SWC1,  UNDEF8,      7},
      {OP_FTYPE, SUBOP_FPADD, 9},
      {OP_FTYPE, SUBOP_FPSUB, 9},
      {OP_FTYPE, SUBOP_FPMUL, 9},
      {OP_FTYPE, SUBOP_FPDIV, 9},
      {OP_FTYPE, SUBOP_FPCEQ, 8},
      {OP_FTYPE, SUBOP_FPCLE, 8},
      {OP_FTYPE, SUBOP_FPCLT, 8},
      {OP_FTYPE, UNDEF8     , 5}, // bc1t / bc1f
      {UNDEF8,   UNDEF8,      8}   // I type

    };

    CpuPipelined(std::shared_ptr<Memory>);
    CpuPipelined(std::shared_ptr<ControlUnit>, std::shared_ptr<Memory>);
    virtual ~CpuPipelined() override;

    virtual bool next_cycle( bool verbose = true ) override;

  private:
    void stage_if( bool verbose = true );
    void stage_id( bool verbose = true );
    void stage_ex( bool verbose = true );
    void stage_mem( bool verbose = true );
    void stage_wb( bool verbose = true );

    // void stage_ex_cop ( bool verbose = true);
    // void stage_mem_cop ( bool verbose = true);
    // void stage_wb_cop ( bool verbose = true);
    //
    // seg_reg_t cop_seg_regs[DIV_DELAY + 2] = {};

    bool pc_write; /* if false, blocks pipeline */
    uint32_t next_pc;
    int flush_pipeline;

    uint32_t forward_register( uint32_t reg, uint32_t reg_value, bool fp_reg = false ) const;
    bool detect_hazard( uint32_t read_reg, bool can_forward, bool fp_reg = false ) const;
        bool process_branch(uint32_t instruction_code,
                        uint32_t rs_value, uint32_t rt_value,
                        uint32_t pc_value);

    /* return false in case of structural hazard */
    /* registers can be written once per cycle */
    bool write_segmentation_register(size_t index, seg_reg_t values);

    seg_reg_t seg_regs[STAGE_COUNT-1] = {};
    uint32_t seg_regs_wrflag = 0;

    seg_reg_t next_seg_regs[STAGE_COUNT-1] = {};

    uint32_t sigmask[STAGE_COUNT-1] = {};

    uint32_t pc_conditional_branch;
    uint32_t pc_instruction_jump;
    uint32_t pc_register_jump;
};

} /* namespace */
#endif
