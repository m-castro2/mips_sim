#ifndef MIPS_SIM_CPU_PIPELINED_H
#define MIPS_SIM_CPU_PIPELINED_H

#define STAGE_IF  0
#define STAGE_ID  1
#define STAGE_EX  2
#define STAGE_MEM 3
#define STAGE_WB  4
#define STAGE_COUNT 5

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

class CpuPipelined : public Cpu
{
  public:
    CpuPipelined(std::shared_ptr<ControlUnit>, std::shared_ptr<Memory>);
    virtual ~CpuPipelined() override;

    virtual bool next_cycle( void ) override;

  private:
    void stage_if( void );
    void stage_id( void );
    void stage_ex( void );
    void stage_mem( void );
    void stage_wb( void );

    bool pc_write; /* if false, blocks pipeline */
    uint32_t next_pc;
    int flush_pipeline;

    uint32_t forward_register( uint32_t reg, uint32_t reg_value ) const;
    bool detect_hazard( uint32_t read_reg, bool can_forward ) const;
    bool process_branch(uint32_t instruction_code,
                        uint32_t rs_value, uint32_t rt_value,
                        uint32_t pc_value);

    seg_reg_t seg_regs[STAGE_COUNT-1] = {};
    seg_reg_t next_seg_regs[STAGE_COUNT-1] = {};

    uint32_t sigmask[STAGE_COUNT-1] = {};
};

} /* namespace */
#endif
