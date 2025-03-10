#ifndef MIPS_SIM_CPU_PIPELINED_H
#define MIPS_SIM_CPU_PIPELINED_H

#include "cpu.h"

/* segmentation register types */
#define IF_ID  0
#define ID_EX  1
#define EX_MEM 2
#define MEM_WB 3


#define MAX_DIAGRAM_SIZE 500

#define X -1

namespace mips_sim
{

  const std::string stage_names[] = {"IF",  "ID",  "EX", "MEM", "WB"};


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
      {X, 0, X, 0, 1, X, 0, 1, 0, 0, X, X, 1, 0, 0, X}, // 11 SWC1
      {X, 0, X, 0, 0, X, 0, 0, 0, 0, X, X, 0, 2, 0, X}, // 12 SYSCALL
      {X, 0, X, 0, 0, X, 0, 0, 1, 0, X, X, 0, 2, 0, X}, // 13 MULT/DIV/
      {0} // end
    };

    static constexpr ctrl_dir_t uc_ctrl_dir[] =
    {
    // OPCODE    SUBOP         JUMP1    JUMP2    JUMP4 
      {OP_RTYPE, SUBOP_JR,       2, UNDEF32, UNDEF32},
      {OP_RTYPE, SUBOP_JALR,     4, UNDEF32, UNDEF32},
      {OP_RTYPE, SUBOP_SYSCALL, 12, UNDEF32, UNDEF32},
      {OP_RTYPE, SUBOP_MULT,    13, UNDEF32, UNDEF32},
      {OP_RTYPE, SUBOP_DIV,     13, UNDEF32, UNDEF32},
      {OP_RTYPE, SUBOP_MULTU,   13, UNDEF32, UNDEF32},
      {OP_RTYPE, SUBOP_DIVU,    13, UNDEF32, UNDEF32},
      {OP_RTYPE, UNDEF8,         0, UNDEF32, UNDEF32},
      {OP_J,     UNDEF8,         1, UNDEF32, UNDEF32},
      {OP_JAL,   UNDEF8,         3, UNDEF32, UNDEF32},
      {OP_BEQ,   UNDEF8,         5, UNDEF32, UNDEF32},
      {OP_BNE,   UNDEF8,         5, UNDEF32, UNDEF32},
      {OP_LW,    UNDEF8,         6, UNDEF32, UNDEF32},
      {OP_LWC1,  UNDEF8,        10, UNDEF32, UNDEF32},
      {OP_SW,    UNDEF8,         7, UNDEF32, UNDEF32},
      {OP_SWC1,  UNDEF8,        11, UNDEF32, UNDEF32},      
      {OP_FTYPE, SUBOP_FPADD,    9, UNDEF32, UNDEF32},
      {OP_FTYPE, SUBOP_FPSUB,    9, UNDEF32, UNDEF32},
      {OP_FTYPE, SUBOP_FPMUL,    9, UNDEF32, UNDEF32},
      {OP_FTYPE, SUBOP_FPDIV,    9, UNDEF32, UNDEF32},
      {OP_FTYPE, SUBOP_FPCEQ,    8, UNDEF32, UNDEF32},
      {OP_FTYPE, SUBOP_FPCLE,    8, UNDEF32, UNDEF32},
      {OP_FTYPE, SUBOP_FPCLT,    8, UNDEF32, UNDEF32},
      {OP_FTYPE, UNDEF8,         5, UNDEF32, UNDEF32}, // bc1t / bc1f
      {UNDEF8,   UNDEF8,         8, UNDEF32, UNDEF32}  // I type
    };

    //CpuPipelined(std::shared_ptr<Memory>);
    CpuPipelined(std::shared_ptr<Memory>,
                 std::shared_ptr<ControlUnit> = nullptr,
                 int branch_type = BRANCH_NON_TAKEN,
                 int branch_stage = STAGE_ID,
                 bool has_forwarding_unit = true,
                 bool has_hazard_detection_unit = true);
    virtual ~CpuPipelined() override;

    void get_current_state(uint32_t *);
    const uint32_t * const * get_diagram( void ) const;

    virtual bool next_cycle( std::ostream & = std::cout ) override;
    virtual void print_diagram( std::ostream & = std::cout ) const override;
    virtual void print_status( std::ostream & = std::cout ) const override;
    virtual void reset( bool reset_data_memory = true,
                        bool reset_text_memory = true ) override;

    virtual void enable_hazard_detection_unit( bool );
    virtual void enable_forwarding_unit( bool );
    virtual void set_branch_stage( int );
    virtual void set_branch_type( int );

    /* signals sorted in reverse order */
    signal_t signals_ID[10] = {
      SIG_MEM2REG, SIG_REGBANK, SIG_REGWRITE, // WB stage
      SIG_MEMREAD, SIG_MEMWRITE, // MEM stage
      SIG_BRANCH, SIG_PCSRC, SIG_ALUSRC, SIG_ALUOP, SIG_REGDST}; // EX stage
  
  protected:

    uint32_t **diagram;

  private:

    bool pc_write; /* if false, blocks pipeline */
    uint32_t next_pc;
    int flush_pipeline;
    seg_reg_t seg_regs[STAGE_COUNT-1] = {};
    uint32_t seg_regs_wrflag = 0;

    seg_reg_t next_seg_regs[STAGE_COUNT-1] = {};

    uint32_t sigmask[STAGE_COUNT-1] = {};

    uint32_t pc_conditional_branch;
    uint32_t pc_instruction_jump;
    uint32_t pc_register_jump;

    uint32_t loaded_instruction_index;
    uint32_t current_state[STAGE_COUNT] = {UNDEF32,UNDEF32,UNDEF32,UNDEF32,UNDEF32};

    void stage_if( std::ostream &out = std::cout );
    void stage_id( std::ostream &out = std::cout );
    void stage_ex( std::ostream &out = std::cout );
    void stage_mem( std::ostream &out = std::cout );
    void stage_wb( std::ostream &out = std::cout );

    size_t get_current_instruction(size_t stage) const;

    // void stage_ex_cop ( bool verbose = true);
    // void stage_mem_cop ( bool verbose = true);
    // void stage_wb_cop ( bool verbose = true);
    //
    // seg_reg_t cop_seg_regs[DIV_DELAY + 2] = {};

    uint32_t forward_register( uint32_t reg, uint32_t reg_value,
                               bool fp_reg = false,
                               std::ostream &out = std::cout ) const;
    bool detect_hazard( uint32_t read_reg, bool can_forward,
                        bool fp_reg = false ) const;

    bool process_branch(uint32_t instruction_code,
                        uint32_t rs_value, uint32_t rt_value,
                        uint32_t pc_value);

    /* return false in case of structural hazard */
    /* registers can be written once per cycle */
    bool write_segmentation_register(size_t index, seg_reg_t values);
};

} /* namespace */

#undef X
#endif
