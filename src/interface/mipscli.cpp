#include "mipscli.h"
//#include "cli/clifilesession.h"
#include "cli/clilocalsession.h"
#include "cli/loopscheduler.h"
#include "../assembler/mips_assembler.h"
#include "../cpu/cpu_multi.h"
#include "../cpu/cpu_pipelined.h"
#include "../utils.h"
#include "../exception.h"

using MainScheduler = cli::LoopScheduler;
using namespace cli;
using namespace std;

// b must be in lower case
static bool match(string a, string b)
{
  transform(a.begin(), a.end(), a.begin(), ::tolower);

  return b.find(a) == 0;
}

namespace mips_sim
{
  MipsCli::MipsCli( void )
  {
    mem = shared_ptr<Memory>(new Memory());
    file_loaded = false;

    auto rootMenu = make_unique< Menu >( "mips" );
    rootMenu -> Insert(
            "hello",
            [](std::ostream& out){ out << "Hello, world\n"; },
            "Print hello world" );
    rootMenu -> Insert(
            "status",
            [this](std::ostream& out){ show_status( out ); },
            "Print status" );
    rootMenu -> Insert(
            "set_cpu",
            [this](std::ostream& out, std::string type){ set_cpu(type, out); },
            "Set cpu [multicycle|pipelined]" );
    rootMenu -> Insert(
            "load",
            [this](std::ostream& out, std::string filename)
            {
              file_loaded = load_file(filename, out);
              if (file_loaded)
              {
                asm_filename = filename;
                if (cpu != nullptr)
                {
                  reset_cpu(out);
                }
              }
             },
            "Load MIPS source file" );
    rootMenu -> Insert(
            "color",
            [](std::ostream& out, bool enable)
              {
                if (enable)
                  SetColor();
                else
                  SetNoColor();
              },
            "Enable colors in the cli [true/false]" );

    auto cpuMenu = make_unique< Menu >( "cpu" );
    cpuMenu -> Insert(
            "status",
            [this](std::ostream& out)
              {
                if (cpu != nullptr)
                {
                  cpu->print_status(out);
                }
                else
                {
                  out << "[error] CPU not set" << endl;
                }
              },
            "Print the CPU status" );
    cpuMenu -> Insert(
            "set",
            [this](std::ostream& out, std::string type){ set_cpu(type, out); },
            "Set CPU [multicycle|pipelined]" );
    cpu_menu = cpuMenu.get();
    rootMenu -> Insert( move(cpuMenu) );

    auto runMenu = make_unique< Menu >( "run" );
    runMenu -> Insert(
            "run",
            [this](std::ostream& out) { run( out ); },
            "Run the code" );
    runMenu -> Insert(
            "n",
            [this](std::ostream& out) { next_cycle( out ); },
            "Next cycle" );
    runMenu -> Insert(
            "p",
            [this](std::ostream& out) { prev_cycle( out ); },
            "Previous cycle" );
    runMenu -> Insert(
            "reset",
            [this](std::ostream& out) { reset_cpu( out ); },
            "Reset to first cycle" );
    runMenu -> Insert(
            "mem",
            [this](std::ostream& out) { show_memory( out ); },
            "Show data memory" );
    runMenu -> Insert(
            "regs",
            [this](std::ostream& out) { show_registers( out ); },
            "Show registers" );
    runMenu -> Insert(
            "dia",
            [this](std::ostream& out) { show_diagram( out ); },
            "Show execution diagram" );
    rootMenu -> Insert( move(runMenu) );

    cl_interface = unique_ptr<Cli>(new Cli( move(rootMenu) ));

    // global exit action
    cl_interface->ExitAction( [](auto& out){ out << "Goodbye!\n"; } );
  }

  bool MipsCli::ready( void ) const
  {
    return cpu != nullptr && file_loaded;
  }

  void MipsCli::show_status( ostream & out ) const
  {
    if (cpu == nullptr)
    {
      out << "Cpu not ready. Call \'set_cpu\' multicycle|pipelined" << endl;
    }
    else
    {
      out << endl;
      out << "CPU: ----------------------------" << endl;
      cpu->print_status(out);
      out << endl;
    }

    if (!file_loaded)
    {
      out << "MIPS file not ready. Call \'load <filename>\'" << endl;
    }
    else
    {
      out << endl;
      out << "File: " << asm_filename << endl;
      out << endl;
    }
  }

  void MipsCli::remove_cpu_handlers( void )
  {
    for (CmdHandler handler : cpu_handlers)
      handler.Remove();
  }

  bool MipsCli::set_cpu( const string & cpu_type, ostream & out )
  {
    if (match(cpu_type, "pipelined"))
    {
      cpu = unique_ptr<Cpu>(new CpuPipelined(mem));
      cpu->print_status(out);

      remove_cpu_handlers();
      cpu_handlers.push_back(cpu_menu->Insert(
              "hdu",
              [this](ostream& out, bool enabled){ dynamic_cast<CpuPipelined&>(*cpu).enable_hazard_detection_unit(enabled); },
              "Sets/Unsets the hazard detection unit" ));
      cpu_handlers.push_back(cpu_menu->Insert(
              "forward",
              [this](ostream& out, bool enabled){ dynamic_cast<CpuPipelined&>(*cpu).enable_forwarding_unit(enabled); },
              "Sets/Unsets the forwarding unit" ));
      cpu_handlers.push_back(cpu_menu->Insert(
              "branch",
              [this](ostream& out, string strat)
                {
                  int branch_type = UNDEF32;
                  if (match(strat, "fixed"))
                    branch_type = BRANCH_NON_TAKEN;
                  else if (match(strat, "flushed"))
                    branch_type = BRANCH_FLUSH;
                  else if (match(strat, "delayed"))
                      branch_type = BRANCH_DELAYED;

                  dynamic_cast<CpuPipelined&>(*cpu).set_branch_type(branch_type);
                },
              "Sets the branch processing technique: fixed | flush | delayed" ));
      cpu_handlers.push_back(cpu_menu->Insert(
              "branch_stage",
              [this](ostream& out, string stage)
                {
                  int branch_stage = UNDEF32;
                  if (match(stage, "id"))
                    branch_stage = STAGE_ID;
                  else if (match(stage, "mem"))
                    branch_stage = STAGE_MEM;

                  dynamic_cast<CpuPipelined&>(*cpu).set_branch_stage(branch_stage);
                },
              "Sets the branch stage: ID | MEM" ));
    }
    else if (match(cpu_type, "multicycle"))
    {
      cpu = unique_ptr<Cpu>(new CpuMulti(mem));
      cpu->print_status(out);

      remove_cpu_handlers();
    }
    else
    {
      out << "[error] undefined CPU. Use \"multicycle\" or \"pipelined\"" << endl;
      return false;
    }

    return true;
  }

  bool MipsCli::load_file( const string & filename, ostream & out )
  {
    if (!Utils::file_exists(filename))
    {
      cerr << "[error] file " << filename << " does not exist" << endl;
      return false;
    }

    try {
      if (assemble_file(filename.c_str(), mem) != 0)
      {
        cerr << "[error] failed parsing the input file" << endl;
        return 1;
      }

      out << "File read OK" << endl;
      show_memory( out );

      mem->snapshot(MEM_TEXT_REGION);
      mem->snapshot(MEM_DATA_REGION);
    }
    catch(int e)
    {
      cerr << "EXCEPTION " << e << ": " << err_msg;
      if (err_v)
        cerr << " [0x" << Utils::hex32(err_v) << "]";
      cerr << endl;
      return false;
    }

    return true;
  }

  int MipsCli::run( ostream & out)
  {
    if (!ready())
    {
      cerr << "[error] CPU is not ready. Call \'status\' from main menu" << endl;
      return 1;
    }

    try
    {
      for (size_t i = 0; cpu->is_ready() ; i++)
      {
        cpu->next_cycle(out);
      }
    }
    catch(int e)
    {
      cerr << "EXCEPTION " << e << ": " << err_msg;
      if (err_v)
        cerr << " [0x" << Utils::hex32(err_v) << "]";
      cerr << endl;
    }
    return 0;
  }

  int MipsCli::reset_cpu( std::ostream & out )
  {
    if (cpu == nullptr)
    {
      cerr << "[error] CPU not set" << endl;
      return 1;
    }

    cpu->reset(true, true);

    return 0;
  }

  int MipsCli::next_cycle( std::ostream & out )
  {
    if (!ready())
    {
      cerr << "[error] CPU is not ready. Call \'status\' from main menu" << endl;
      return 1;
    }

    if (!cpu->is_ready())
    {
      cerr << "Program done" << endl;
      return 0;
    }

    try
    {
      cpu->next_cycle( out );
    }
    catch(int e)
    {
      cerr << "EXCEPTION " << e << ": " << err_msg;
      if (err_v)
        cerr << " [0x" << Utils::hex32(err_v) << "]";
      cerr << endl;
    }

    return 0;
  }

  int MipsCli::prev_cycle( std::ostream & out )
  {
    if (!ready())
    {
      cerr << "[error] CPU is not ready. Call \'status\' from main menu" << endl;
      return 1;
    }

    if (cpu->get_cycle() == 0)
    {
      cerr << "[error] already at first cycle" << endl;
      return 1;
    }

    try
    {
      cpu->run_to_cycle(cpu->get_cycle()-1, out);
    }
    catch(int e)
    {
      cerr << "EXCEPTION " << e << ": " << err_msg;
      if (err_v)
        cerr << " [0x" << Utils::hex32(err_v) << "]";
      cerr << endl;
    }

    return 0;
  }

  int MipsCli::show_memory( std::ostream & out )
  {
    out << "Text region:" << endl;
    mem->print_memory(MEM_TEXT_START, MEM_TEXT_SIZE, out);
    out << "Data region:" << endl;
    mem->print_memory(MEM_DATA_START, MEM_DATA_SIZE, out);
    return 0;
  }

  int MipsCli::show_registers( std::ostream & out )
  {
    if (cpu == nullptr)
    {
      cerr << "[error] CPU not set" << endl;
      return 1;
    }

    try
    {
      cpu->print_registers(out);
    }
    catch(int e)
    {
      cerr << "EXCEPTION " << e << ": " << err_msg;
      if (err_v)
        cerr << " [0x" << Utils::hex32(err_v) << "]";
      cerr << endl;
    }
    return 0;
  }

  int MipsCli::show_diagram( std::ostream & out )
  {
    if (!ready())
    {
      cerr << "[error] CPU is not ready. Call \'status\' from main menu" << endl;
      return 1;
    }

    try
    {
      cpu->print_diagram(out);
    }
    catch(int e)
    {
      cerr << "EXCEPTION " << e << ": " << err_msg;
      if (err_v)
        cerr << " [0x" << Utils::hex32(err_v) << "]";
      cerr << endl;
    }
    return 0;
  }

  void MipsCli::launch( void )
  {
    cout << endl << "MIPS Simulator Command Line Interface" << endl;

    MainScheduler scheduler;
    CliLocalTerminalSession localSession(*cl_interface, scheduler, std::cout, 200);
    localSession.ExitAction(
        [&scheduler](auto& out) // session exit action
        {
            out << "Closing App...\n";
            scheduler.Stop();
        }
    );

    scheduler.Run();
  }
} /* namespace */
