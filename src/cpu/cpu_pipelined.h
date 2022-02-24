#ifndef MIPS_SIM_CPU_PIPELINED_H
#define MIPS_SIM_CPU_PIPELINED_H

#define HAS_FORWARDING_UNIT       true
#define HAS_HAZARD_DETECTION_UNIT true

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

#include "cpu.h"

namespace mips_sim
{

/* segmentation register */
struct seg_reg_t {
  uint32_t data[32];
};

class CpuPipelined : public Cpu
{
  public:
    CpuPipelined(std::shared_ptr<ControlUnit>, std::shared_ptr<Memory>);
    virtual ~CpuPipelined();

    virtual bool next_cycle( void );

  private:
    void stage_if( void );
    void stage_id( void );
    void stage_ex( void );
    void stage_mem( void );
    void stage_wb( void );

    bool pc_write; /* if false, blocks pipeline */

    uint32_t forward_register( uint32_t reg, uint32_t reg_value ) const;
    bool detect_hazard( uint32_t reg ) const;

    seg_reg_t seg_regs[STAGE_COUNT-1] = {};
    seg_reg_t next_seg_regs[STAGE_COUNT-1] = {};
    std::string instruction_name[STAGE_COUNT] = { "nop", "nop", "nop", "nop", "nop" };

    uint32_t sigmask[STAGE_COUNT-1] = {};
};

} /* namespace */
#endif
