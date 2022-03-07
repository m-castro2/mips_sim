#ifndef MIPS_SIM_EXCEPTION_H
#define MIPS_SIM_EXCEPTION_H

/* bits [6..31] - Exception type */
/* bits [0..5] - Specific type */
#define MEMORY_EXCEPTION          64
#define MEMORY_ALLOC_EXCEPTION    65
#define MEMORY_READ_EXCEPTION     66
#define MEMORY_WRITE_EXCEPTION    67

#define PARSER_EXCEPTION         128
#define PARSER_UNDEF_EXCEPTION   129

#define CTRL_EXCEPTION           192
#define CTRL_UNDEF_EXCEPTION     193

namespace mips_sim
{

extern uint32_t err_v;
extern int err_no;
extern std::string err_msg;

class Exception
{
public:
  static int e(int exception, std::string message, uint32_t value = 0)
  {
    err_no = exception;
    err_msg = message;
    err_v = value;

    return err_no;
  }
};

} /* namespace */
#endif
