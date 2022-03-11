#ifndef MIPS_SIM_MIPS_CLI_H
#define MIPS_SIM_MIPS_CLI_H

#include "cli/cli.h"
#include "../assembler/mips_assembler.h"
#include "../assembler/mips_parser.hpp"
#include "../cpu/cpu.h"
#include "../mem.h"

#include <memory>
#include <vector>
#include <iostream>

namespace mips_sim
{
  class MipsCli
  {
  public:
    MipsCli( void );
    void launch( void );

  private:

    /* status */
    cli::Menu * cpu_menu;
    std::string asm_filename;
    bool file_loaded;
    std::shared_ptr<Memory> mem;
    std::unique_ptr<Cpu> cpu;
    std::unique_ptr<cli::Cli> cl_interface;

    bool ready( void ) const;

    void show_status( std::ostream & ) const;
    bool set_cpu( const std::string &, std::ostream & );
    bool load_file( const std::string &, std::ostream & );
    int run( std::ostream & );
    int reset_cpu( std::ostream & );
    int next_cycle( std::ostream & );
    int prev_cycle( std::ostream & );
    int show_memory( std::ostream & );
    int show_registers( std::ostream & );
    int show_diagram( std::ostream & );

    void remove_cpu_handlers( void );

    std::vector<cli::CmdHandler> cpu_handlers;
  };
} /* namespace */
#endif
