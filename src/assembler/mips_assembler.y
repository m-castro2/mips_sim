%{
  #include "../utils.h"
  #include "../mem.h"
  #include "assembler.h"

  #include <iostream>
  #include <sstream>
  #include <cstring>
  #include <cassert>
  #include <vector>
  #include <memory>
  #include <algorithm>

  using namespace std;
  using namespace mips_sim;

  int yyparse();
  int assemble_file(FILE *fp, shared_ptr<Memory> memory);

  extern "C"
  {
    #include "mips_parser_lex.hpp"

    int yylex( void );

    void yyerror(const char *str)
    {
      cerr << "error: " << str << endl;
    }

    int yywrap()
    {
      return 1;
    }
  }

  struct Label
  {
    std::string name;
    uint32_t line;
  };

  struct Instruction
  {
    uint32_t instcode;
    uint32_t line;
    bool has_branch_label;
    bool relative;
    int has_data_label; /* 1: high, 2: low */
    std::string label;
  };

  struct Memsection
  {
    string label;
    int type; /* 1: word, 2: float, 3: double, 4: asccii, 5: space */
    uint32_t position;
    uint32_t length;
    vector<string> values;
  };

  static vector<string> values;
  static vector<Label> labels;
  static vector<Instruction> instructions;
  static vector<Memsection> memsections;

  static uint32_t line = 0;
  static uint32_t mem_pos = 0;

%}

%union {
  uint32_t ival;
  char * tval;
}

%destructor { free ($$); } <tval>

%token COMMA
%token SYSCALL
%token TEXT_SECTION
%token DATA_SECTION
%token LAOPCODE
%token LUIOPCODE
%token OBRACKET
%token CBRACKET
%token <tval> DATATAG
%token <tval> LABELTAG
%token <tval> LABELJUMP
%token <tval> REGISTER
%token <tval> IOPCODE
%token <tval> JOPCODE
%token <tval> ROPCODE
%token <tval> IMOPCODE
%token <ival> INTVALUE
%token <tval> FLOATVALUE
%token <tval> TEXTVALUE
%token <ival> EOL

%start program

%%

program: TEXT_SECTION instructions_sequence DATA_SECTION data_sequence
  | DATA_SECTION data_sequence TEXT_SECTION instructions_sequence
  | TEXT_SECTION instructions_sequence;

instructions_sequence:
    instructions_sequence instruction
    {
      line++;
    }
    |
    instruction
    {
      line++;
    };

instruction:
		ROPCODE REGISTER COMMA REGISTER COMMA REGISTER
		{
      stringstream ss;
      ss << $1 << " " << $2 << ", " << $4 << ", " << $6;
      uint32_t instcode = Utils::assemble_instruction(ss.str());
      cout << Utils::hex32(instcode) << " Rop " << ss.str() << endl;
      instructions.push_back({instcode, line, false, false, 0, ""});
      free($1); free($2); free($4); free($6);
		}
	|	IOPCODE REGISTER COMMA REGISTER COMMA INTVALUE
		{
      stringstream ss;
      ss << $1 << " " << $2 << ", " << $4 << ", " << $6;
      uint32_t instcode = Utils::assemble_instruction(ss.str());
      cout << Utils::hex32(instcode) << " Iop " << ss.str() << endl;
      instructions.push_back({instcode, line, false, false, 0, ""});
      free($1); free($2); free($4);
		}
  |	IOPCODE REGISTER COMMA REGISTER COMMA LABELJUMP
		{
      stringstream ss;
      ss << $1 << " " << $2 << ", " << $4 << ", " << $6;
      uint32_t instcode = Utils::assemble_instruction(ss.str());
      cout << Utils::hex32(instcode) << " Bra " << ss.str() << endl;
      instructions.push_back({instcode, line, true, true, 0, $6});
      free($1); free($2); free($4); free($6);
		}
  |	LUIOPCODE REGISTER COMMA INTVALUE
		{
      stringstream ss;
      ss << "lui " << $2 << ", " << $4;
      uint32_t instcode = Utils::assemble_instruction(ss.str());
      cout << Utils::hex32(instcode) << " Lui " << ss.str() << endl;
      instructions.push_back({instcode, line, false, false, 0, ""});
      free($2);
		}
	|	IMOPCODE REGISTER COMMA INTVALUE OBRACKET REGISTER CBRACKET
		{
      stringstream ss;
      ss << $1 << " " << $2 << ", " << $4 << "(" << $6 << ")";
      uint32_t instcode = Utils::assemble_instruction(ss.str());
      cout << Utils::hex32(instcode) << " Mem " << ss.str() << endl;
      instructions.push_back({instcode, line, false, false, 0, ""});
      free($1); free($2); free($6);
		}
	|	JOPCODE LABELJUMP
		{
      stringstream ss;
      ss << $1 << " " << $2;
      uint32_t instcode = Utils::assemble_instruction(ss.str());
      cout << Utils::hex32(instcode) << " Jlb " << ss.str() << endl;
      instructions.push_back({instcode, line, true, false, 0, $2});
      free($1); free($2);
		}
  |	LAOPCODE REGISTER COMMA LABELJUMP
		{
      /* decompose in lui & ori */
      stringstream ss1, ss2;
      ss1 << "lui " << $2 << ", 0";
      ss2 << "ori " << $2 << ", " << $2 << ", 0";
      uint32_t instcode = Utils::assemble_instruction(ss1.str());
      cout << Utils::hex32(instcode) << " La1 " << ss1.str() << endl;
      instructions.push_back({instcode, line, false, false, 1, $4});
      line++;
      instcode = Utils::assemble_instruction(ss2.str());
      cout << Utils::hex32(instcode) << " La2 " << ss2.str() << endl;
      instructions.push_back({instcode, line, false, false, 2, $4});
      free($2);
		}
  |	JOPCODE REGISTER
		{
      stringstream ss;
      ss << $1 << " " << $2;
      uint32_t instcode = Utils::assemble_instruction(ss.str());
      cout << Utils::hex32(instcode) << " Jrg " << ss.str() << endl;
      instructions.push_back({instcode, line, false, false, 0, ""});
      free($1); free($2);
		}
  | SYSCALL
    {
      uint32_t instcode = Utils::assemble_instruction("syscall");
      cout << Utils::hex32(instcode) << " Syscall" << endl;
      instructions.push_back({instcode, line, false, false, 0, ""});
    }
  | LABELTAG
    {
      labels.push_back({$1, line});
      line--;
      free($1);
    }
	;

  data_sequence: data_sequence data_line | data_line;

  data_line:
      LABELTAG DATATAG array_values
      {
        uint32_t last_pos = mem_pos;
        int type = 0;
        reverse(values.begin(), values.end());
        if (!strcmp($2, "word"))
        {
          type = 1;
          cout << "Int array " << $1;
          for (string v : values)
          {
            cout << " " << v;
          }
          cout << endl;
          mem_pos += values.size() * 4;
        }
        else if (!strcmp($2, "float"))
        {
          type = 2;
          cout << "Float array " << $1;
          for (string v : values)
          {
            cout << " " << v;
          }
          cout << endl;
          mem_pos += values.size() * 4;
        }
        else if (!strcmp($2, "double"))
        {
          type = 3;
          cout << "Double array " << $1;
          for (string v : values)
          {
            cout << " " << v;
          }
          cout << endl;
          mem_pos += values.size() * 8;
        }
        else if (!strcmp($2, "asciiz"))
        {
          type = 4;
          assert(values.size() == 1);
          cout << "ASCII " << $1;
          for (string v : values)
          {
            cout << " " << v;
          }
          cout << endl;
          mem_pos += values[0].length();
        }
        else if (!strcmp($2, "space"))
        {
          type = 5;
          assert(values.size() == 1);
          cout << "Space allocation " << $1;
          for (string v : values)
          {
            cout << " " << v;
          }
          cout << endl;
          mem_pos += stoi(values[0]);
        }
        memsections.push_back({$1, type, last_pos, mem_pos-last_pos, values});
        values.clear();
      }
      ;

  array_values:
      FLOATVALUE
      {
        values.push_back($1);
      }
      | FLOATVALUE COMMA array_values
      {
        values.push_back($1);
      }
      | INTVALUE
      {
        values.push_back(to_string($1));
      }
      | INTVALUE COMMA array_values
      {
        values.push_back(to_string($1));
      }
      | TEXTVALUE
      {
        values.push_back($1);
      }
%%

namespace mips_sim
{

int assemble_file(const char filename[], shared_ptr<Memory> memory)
{
  uint32_t next_address;
  uint32_t words_to_mem[2];

  yyin = fopen(filename, "r");
  yyparse();
  fclose(yyin);

  /* rebuild instructions */
  for (Instruction & instruction : instructions)
  {
    if (instruction.has_branch_label)
    {
      for (Label label : labels)
      {
        if (!instruction.label.compare(label.name))
        {
          if (instruction.relative)
          {
            int rel_branch = label.line - instruction.line - 1;
            instruction.instcode |= (static_cast<uint32_t>(rel_branch) & 0xFFFF);
          }
          else
          {
            uint32_t abs_branch = MEM_TEXT_START/4 + static_cast<uint32_t>(label.line);
            instruction.instcode |= (abs_branch & 0x3FFFFFF);
          }
        }
      }
    }
    else if (instruction.has_data_label > 0)
    {
      assert(instruction.has_data_label <= 2);
      for (Memsection mem_section : memsections)
      {
        if (!instruction.label.compare(mem_section.label))
        {
          uint32_t address = MEM_DATA_START + mem_section.position;
          if (instruction.has_data_label == 1)
            instruction.instcode |= (address >> 16 & 0xFFFF);
          else
            instruction.instcode |= (address & 0xFFFF);
        }
      }
    }
  }

  memory->lock();
  /* process data memory sections */
  next_address = MEM_DATA_START;
  for (Memsection memsection : memsections)
  {
    cout << "ALLOCATE " << memsection.length << " Bytes" << endl;
    memory->allocate_space(next_address,
                           memsection.length);
    /* copy data */
    for (string s : memsection.values)
    {
      if (memsection.type == 1)
      {
        memory->mem_write_32(next_address,
                              stoi(s));
        next_address += 4;
      }
      else if (memsection.type == 2)
      {
        Utils::float_to_word(stof(s), words_to_mem);
        memory->mem_write_32(next_address,
                             words_to_mem[0]);
        next_address += 4;
      }
      else if (memsection.type == 3)
      {
        Utils::double_to_word(stod(s), words_to_mem);
        memory->mem_write_32(next_address,
                             words_to_mem[0]);
        memory->mem_write_32(next_address + 4,
                             words_to_mem[1]);
        next_address += 8;
      }
      else if (memsection.type == 4)
      {
        //TODO: Save text
        assert(0);
      }
      else if (memsection.type == 5)
      {
        next_address += stoi(s);
      }
    }
  }

  /* process code memory */
  memory->allocate_space(MEM_TEXT_START,
                         static_cast<uint32_t>((instructions.size()+4) * 4));

  next_address = MEM_TEXT_START;
  for (Instruction instruction : instructions)
  {
    /* Read in the program. */
    memory->mem_write_32(next_address, instruction.instcode);
    next_address += 4;
    //words_read++;

    cout << Utils::hex32(instruction.instcode) << endl;
  }

  return 0;
}

} /* namespace */
