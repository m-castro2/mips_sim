#ifndef MIPS_SIM_GLOBAL_DEFS_H
#define MIPS_SIM_GLOBAL_DEFS_H

#include <string>
#include <cstdint>

#define FORMAT_R 0
#define FORMAT_I 1
#define FORMAT_J 2
#define FORMAT_F 3

#define UNDEF32 0xFFFFFFFF
#define UNDEF16 0xFFFF
#define UNDEF8  0xFF

#define OUT

#define REG_BANK_INT     0
#define REG_BANK_FP      1
#define REG_BANK_SPECIAL 2

/* brahch processing options */
#define BRANCH_FLUSH     0 /* flush pipeline */
#define BRANCH_NON_TAKEN 1 /* fixed non taken */
#define BRANCH_DELAYED   2 /* delayed branch */

/* processor stages */
#define STAGE_IF      0
#define STAGE_ID      1
#define STAGE_EX      2
#define STAGE_MEM     3
#define STAGE_WB      4
#define STAGE_FWB     5
#define STAGE_COUNT   6

/* status keys */
#define KEY_BRANCH_TYPE           "branch-type"
#define KEY_BRANCH_STAGE          "branch-stage"
#define KEY_FORWARDING_UNIT       "has-forwarding-unit"
#define KEY_HAZARD_DETECTION_UNIT "has-hazard-detection-unit"

typedef enum
{
  SUBOP_SLL     = 0x0,
  SUBOP_SRL     = 0x2,
  SUBOP_SRA     = 0x3,
  SUBOP_SLLV    = 0x4,
  SUBOP_SRLV    = 0x6,
  SUBOP_SRAV    = 0x7,
  SUBOP_JR      = 0x8,
  SUBOP_JALR    = 0x9,
  SUBOP_SYSCALL = 0xc,
  SUBOP_MULT    = 0x18,
  SUBOP_MULTU   = 0x19,
  SUBOP_DIV     = 0x1a,
  SUBOP_DIVU    = 0x1b,
  SUBOP_ADD     = 0x20,
  SUBOP_ADDU    = 0x21,
  SUBOP_SUB     = 0x22,
  SUBOP_SUBU    = 0x23,
  SUBOP_AND     = 0x24,
  SUBOP_OR      = 0x25,
  SUBOP_XOR     = 0x26,
  SUBOP_NOR     = 0x27,
  SUBOP_SLT     = 0x2A,
  SUBOP_SLTU    = 0x2B,
  SUBOP_MFHI    = 0x10,
  SUBOP_MTHI    = 0x11,
  SUBOP_MFLO    = 0x12,
  SUBOP_MTLO    = 0x13
} subopcode_t;

typedef enum
{
  SUBOP_FPADD = 0x0,
  SUBOP_FPSUB = 0x1,
  SUBOP_FPMUL = 0x2,
  SUBOP_FPDIV = 0x3,
  SUBOP_FPMOV = 0x6,
  SUBOP_FPCEQ = 0x32,
  SUBOP_FPCLT = 0x3C,
  SUBOP_FPCLE = 0x3E
} fpsubopcode_t;

/* primary opcodes */
#define OP_COUNT 26

typedef enum
{
  OP_RTYPE = 0x00,
  OP_J     = 0x02,
  OP_JAL   = 0x03,
  OP_BEQ   = 0x04,
  OP_BNE   = 0x05,
  OP_BLEZ  = 0x06,
  OP_BGTZ  = 0x07,
  OP_ADDI  = 0x08,
  OP_ADDIU = 0x09,
  OP_SLTI  = 0x0a,
  OP_SLTIU = 0x0b,
  OP_ANDI  = 0x0c,
  OP_ORI   = 0x0d,
  OP_XORI  = 0x0e,
  OP_LUI   = 0x0f,
  OP_FTYPE = 0x11,
  OP_LB    = 0x20,
  OP_LH    = 0x21,
  OP_LW    = 0x23,
  OP_LBU   = 0x24,
  OP_LHU   = 0x25,
  OP_SB    = 0x28,
  OP_SH    = 0x29,
  OP_SW    = 0x2b,
  OP_LWC1  = 0x31,
  OP_SWC1  = 0x39
} opcode_t;

/*
 * R format: opcode(6) rs(5) rt(5) rd(5) shamt(5) funct(6)
 * I format: opcode(6) rs(5) rt(5) imm(16)
 * J format: opcode(6) addr(6)
 * F format: opcode(6) cop(5) rs(5) rt(5) rd(5) funct(6)
 */
typedef struct {
    uint32_t code;
    uint8_t opcode;
    bool fp_op; // FP instruction
    uint8_t cop; // for FP instructions
    uint8_t rs; // or fs
    uint8_t rt; // or ft
    uint8_t rd; // or fd
    uint8_t shamt;
    uint8_t funct;
    uint16_t addr_i;
    uint32_t addr_j;
} instruction_t;

typedef struct {
    uint32_t opcode;
    uint32_t subopcode; // for R format
    std::string opname;
    uint32_t symbols_count;
    int format;
} instruction_format_t;

typedef struct {
    uint32_t reg_index;
    std::string regname_generic;
    std::string regname_int;
    std::string regname_fp;
    std::string regname_special;
} register_format_t;

typedef struct {
    uint32_t id;
    uint32_t pc;
    uint32_t code;
} running_instruction_t;

const register_format_t registers_def[]
{
  {  0, "$0", "$0",  "$f0", "pc" },    {  1, "$1", "$at", "$f1", "status" },
  {  2, "$2", "$v0", "$f2", "hi" },    {  3, "$3", "$v1", "$f3", "lo" },
  {  4, "$4", "$a0", "$f4", "epc" },   {  5, "$5", "$a1", "$f5", "badvaddr" },
  {  6, "$6", "$a2", "$f6", "cause" }, {  7, "$7", "$a3", "$f7", "" },
  {  8, "$8", "$t0", "$f8", "" },      {  9, "$9", "$t1", "$f9", "" },   { 10, "$10", "$t2", "$f10", "" }, { 11, "$11", "$t3", "$f11", "" },
  { 12, "$12", "$t4", "$f12", "" },    { 13, "$13", "$t5", "$f13", "" }, { 14, "$14", "$t6", "$f14", "" }, { 15, "$15", "$t7", "$f15", "" },
  { 16, "$16", "$s0", "$f16", "" },    { 17, "$17", "$s1", "$f17", "" }, { 18, "$18", "$s2", "$f18", "" }, { 19, "$19", "$s3", "$f19", "" },
  { 20, "$20", "$s4", "$f20", "" },    { 21, "$21", "$s5", "$f21", "" }, { 22, "$22", "$s6", "$f22", "" }, { 23, "$23", "$s7", "$f23", "" },
  { 24, "$24", "$t8", "$f24", "" },    { 25, "$25", "$t9", "$f25", "" }, { 26, "$26", "$k0", "$f26", "" }, { 27, "$27", "$k1", "$f27", "" },
  { 28, "$28", "$gp", "$f28", "" },    { 29, "$29", "$sp", "$f29", "" }, { 30, "$30", "$fp", "$f30", "" }, { 31, "$31", "$ra", "$f31", "" }
};

const instruction_format_t instructions_def[]
{
  { OP_RTYPE, SUBOP_ADD,     "add",     4, FORMAT_R },
  { OP_RTYPE, SUBOP_SLL ,    "sll",     3, FORMAT_R },
  { OP_RTYPE, SUBOP_SRL ,    "srl",     3, FORMAT_R },
  { OP_RTYPE, SUBOP_SRA ,    "sra",     3, FORMAT_R },
  { OP_RTYPE, SUBOP_SLLV,    "sllv",    4, FORMAT_R },
  { OP_RTYPE, SUBOP_SRLV,    "srlv",    4, FORMAT_R },
  { OP_RTYPE, SUBOP_SRAV,    "srav",    4, FORMAT_R },
  { OP_RTYPE, SUBOP_JR  ,    "jr",      2, FORMAT_R },
  { OP_RTYPE, SUBOP_JALR,    "jalr",    2, FORMAT_R },
  { OP_RTYPE, SUBOP_SYSCALL, "syscall", 1, FORMAT_R },
  { OP_RTYPE, SUBOP_MULT,    "mult",    3, FORMAT_R },
  { OP_RTYPE, SUBOP_MULTU,   "multu",   3, FORMAT_R },
  { OP_RTYPE, SUBOP_DIV,     "div",     3, FORMAT_R },
  { OP_RTYPE, SUBOP_DIVU,    "divu",    3, FORMAT_R },
  { OP_RTYPE, SUBOP_ADD,     "add",     4, FORMAT_R },
  { OP_RTYPE, SUBOP_ADDU,    "addu",    4, FORMAT_R },
  { OP_RTYPE, SUBOP_SUB,     "sub",     4, FORMAT_R },
  { OP_RTYPE, SUBOP_SUBU,    "subu",    4, FORMAT_R },
  { OP_RTYPE, SUBOP_AND,     "and",     4, FORMAT_R },
  { OP_RTYPE, SUBOP_OR,      "or",      4, FORMAT_R },
  { OP_RTYPE, SUBOP_XOR,     "xor",     4, FORMAT_R },
  { OP_RTYPE, SUBOP_NOR,     "nor",     4, FORMAT_R },
  { OP_RTYPE, SUBOP_SLT,     "slt",     4, FORMAT_R },
  { OP_RTYPE, SUBOP_SLTU,    "sltu",    4, FORMAT_R },
  { OP_RTYPE, SUBOP_MFHI,    "mfhi",    4, FORMAT_R },
  { OP_RTYPE, SUBOP_MTHI,    "mthi",    4, FORMAT_R },
  { OP_RTYPE, SUBOP_MFLO,    "mflo",    4, FORMAT_R },
  { OP_RTYPE, SUBOP_MTLO,    "mtlo",    4, FORMAT_R },
  { OP_J,     0,            "j",        2, FORMAT_J },
  { OP_JAL,   0,            "jal",      2, FORMAT_J },
  { OP_BEQ,   0,            "beq",      4, FORMAT_I },
  { OP_BNE,   0,            "bne",      4, FORMAT_I },
  { OP_BLEZ,  0,            "blez",     4, FORMAT_I },
  { OP_BGTZ,  0,            "bgtz",     4, FORMAT_I },
  { OP_ADDI,  0,            "addi",     4, FORMAT_I },
  { OP_ADDIU, 0,            "addiu",    4, FORMAT_I },
  { OP_SLTI,  0,            "slti",     4, FORMAT_I },
  { OP_SLTIU, 0,            "sltiu",    4, FORMAT_I },
  { OP_ANDI,  0,            "andi",     4, FORMAT_I },
  { OP_ORI,   0,            "ori",      4, FORMAT_I },
  { OP_XORI,  0,            "xori",     4, FORMAT_I },
  { OP_LUI,   0,            "lui",      3, FORMAT_I },
  { OP_LB,    0,            "lb",       4, FORMAT_I },
  { OP_LH,    0,            "lh",       4, FORMAT_I },
  { OP_LW,    0,            "lw",       4, FORMAT_I },
  { OP_LBU,   0,            "lbu",      4, FORMAT_I },
  { OP_LHU,   0,            "lhu",      4, FORMAT_I },
  { OP_SB,    0,            "sb",       4, FORMAT_I },
  { OP_SH,    0,            "sh",       4, FORMAT_I },
  { OP_SW,    0,            "sw",       4, FORMAT_I },
  { OP_LWC1,  0,            "lwc1",     4, FORMAT_I },
  { OP_SWC1,  0,            "swc1",     4, FORMAT_I },
  { OP_FTYPE, SUBOP_FPADD,  "add.",     4, FORMAT_F },
  { OP_FTYPE, SUBOP_FPSUB,  "sub.",     4, FORMAT_F },
  { OP_FTYPE, SUBOP_FPMUL,  "mul.",     4, FORMAT_F },
  { OP_FTYPE, SUBOP_FPDIV,  "div.",     4, FORMAT_F },
  { OP_FTYPE, SUBOP_FPMOV,  "mov.",     3, FORMAT_F },
  { OP_FTYPE, SUBOP_FPCLE,  "c.le.",    3, FORMAT_F },
  { OP_FTYPE, SUBOP_FPCLT,  "c.lt.",    3, FORMAT_F },
  { OP_FTYPE, SUBOP_FPCEQ,  "c.eq.",    3, FORMAT_F },
  { OP_FTYPE, 0,            "bc1t",     2, FORMAT_F }, // Special case
  { OP_FTYPE, 0,            "bc1f",     2, FORMAT_F }, // Special case
  {        0,         0,     "nop",     1, FORMAT_R }
};

/* segmentation register */
struct seg_reg_t {
  uint32_t data[32];
};

/* data through segmentation registers */
#define SR_INSTRUCTION  0
#define SR_IID          1
#define SR_PC           2
#define SR_SIGNALS      3
#define SR_OPCODE       4
#define SR_RS           5
#define SR_RT           6
#define SR_RD           7
#define SR_SHAMT        8
#define SR_FUNCT        9
#define SR_RSVALUE     10
#define SR_RTVALUE     11
#define SR_ADDR_I      12
#define SR_REGDEST     13
#define SR_ALUOUTPUT   14
#define SR_ALUZERO     15
#define SR_RELBRANCH   16
#define SR_UJUMP       17
#define SR_JUMPADDR    18
#define SR_WORDREAD    19
#define SR_V0          20
#define SR_FPPRECISION 21 // 0 single, 1 double
#define SR_FPRSVALUEUPPER 22
#define SR_FPRTVALUEUPPER 23
#define SR_FPOUTPUTUPPER  24

// special register
#define SPECIAL_PC       "pc"
#define SPECIAL_STATUS   "status"
#define SPECIAL_HI       "hi"
#define SPECIAL_LO       "lo"
#define SPECIAL_EPC      "epc"
#define SPECIAL_BADVADDR "badvaddr"
#define SPECIAL_CAUSE    "cause"

typedef struct {
    uint32_t id;
    std::string message;
    uint32_t value;
    int syscall_id;
} syscall_struct_t;

typedef struct {
    uint32_t value;
    bool ready;
    bool just_finished;
    int cycle;
} fpu_forwarding_value_t;

typedef struct {
    int id;
    std::string message;
    uint32_t address;
} error_exception_t;

#endif
