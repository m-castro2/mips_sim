/* Code derived from James Mak's MIPS assembler
 * (c) James Mak 2016
 */

#include "assembler.h"
#include "../utils.h"

#include <cassert>
#include <iostream>
#include <iomanip>
#include <fstream>
#include <sstream>

using namespace std;

namespace mips_sim
{

Assembler::Assembler()
{

}

/* Opens the file and reads it, creates a table of tokens (no spaces) */
int Assembler::open_file(std::string filename)
{
  ifstream file;
  file.open(filename.c_str());

  if(!file.is_open()) //If the file does not exist the program crashes.
  {
    cerr << "File does not exist / failed to open file: " << filename << endl;
    return EXIT_FAILURE;
  }

  else
  {
    char temp;

    while (!file.eof())
    {
      file.get(temp);
      tokens.push_back(temp);
      token_size++;
    }
    token_size--;

    for(uint32_t i = 0; i < token_size; i++)
    {
      cout << tokens[i] << "";
    }
    cout << endl;
  }

  return EXIT_SUCCESS;
}

/* organize the symbols vector */
void Assembler::setup_symbols()
{
  for(uint32_t i = 0; i < token_size; i++)
  {
    token = tokens[i];
    if(isalpha(token))
    {
      consume_identifier(token, i);
    }
    if(token == '$')
    {
      consume_register(token, i);
    }
    if(isdigit(token) || token == '-')
    {
      consume_digit(token, i);
    }
  }
}

void Assembler::setup_code()
{
  uint32_t line_counter = 0;
  uint32_t symbols_count = static_cast<uint32_t>(symbols.size());
  uint32_t symbol_counter = 0;
  uint32_t instruction;
  uint32_t label_counter = 0;

  if (instructions.size())
    instructions.clear();

  if (symbols.empty())
    setup_symbols();

  for(uint32_t i = 0; i < symbols_count; i++)
  {
    /* first pass: record branch/jump addresses */
    first_pass(symbols_count, symbol_counter, line_counter, label_counter);
  }

  symbol_counter = 0;
  line_counter = 0;

  /* second pass: build actual instructions */
  for(uint32_t i = 0; i < 255; i++)
  {
    if((symbol_counter == symbols_count) && (line_counter != 255))
      break;

    instruction = build_instruction(i,symbols_count, symbol_counter,
                                    label_counter, line_counter);
    instructions.push_back(instruction);
  }
}

uint32_t Assembler::load_code(shared_ptr<Memory> memory)
{
  uint32_t i = 0, words_read = 0;

  memory->allocate_space(MEM_TEXT_START,
                         static_cast<uint32_t>((instructions.size()+4) * 4));
  memory->lock();

  /* Read in the program. */
  for (uint32_t word : instructions)
  {
    memory->mem_write_32(static_cast<uint32_t>(MEM_TEXT_START + i), word);
    i += 4;
    words_read++;
  }

  return words_read;
}

/* print out the assembled instructions */
void Assembler::print_file(string output_file)
{
  ofstream oFile;
  oFile.open(output_file);

  if (instructions.empty())
    setup_code();

  for (uint32_t instruction : instructions)
    oFile << setw(8) << setfill('0') << hex << instruction << endl;

  oFile << endl;
  oFile.close();
}

uint16_t Assembler::pop_immediate(uint32_t &symbols_count, uint32_t &symbol_counter)
{
  stringstream temp;
  uint16_t imm_value;
  if(symbol_counter >= symbols_count)
  {
    assert(0);
    return UNDEF16;
  }

  symbol_counter++;
  temp << hex << symbols[symbol_counter];
  temp >> imm_value;

  return imm_value;
}

/* The branchTable function handles the branch address for branch instructions. */
uint16_t Assembler::pop_branch_offset(uint32_t &symbols_count, uint32_t &symbol_counter,
                              uint32_t &label_counter, uint32_t &line_counter)
{
  uint32_t offset = UNDEF32;

  if(symbol_counter >= symbols_count) //Check for overflow of 'symbols' vector.
  {
      assert(0);
      return UNDEF16;
  }

  symbol_counter++;
  string temp_symbol = symbols[symbol_counter];

  for(uint32_t i = 0; i < label_counter; i++)
  {
    if(label[i].name == symbols[symbol_counter])
    {
      if(label[i].address >  line_counter)
      {
        offset = static_cast<uint32_t>((label[i].address - line_counter) - 1);
      }
      else
      {
        offset = static_cast<uint32_t>(-((line_counter - label[i].address) + 1));
      }
      break;
    }
  }

  if (offset == UNDEF32)
  {
    cerr << "Error: Cannot find label " << symbols[symbol_counter] << " :: line " << line_counter << endl;
    assert(0);
  }

  return static_cast<uint16_t>(offset);
}

uint8_t Assembler::pop_register(uint32_t &symbols_count, uint32_t &symbol_counter,
                                  uint32_t &line_counter)
{
  uint8_t reg_id;

  if(symbol_counter >= symbols_count)
  {
    assert(0);
    return UNDEF8;
  }

  symbol_counter++;

  /* find the register id */
  reg_id = Utils::find_register_by_name(symbols[symbol_counter]);
  if (reg_id == UNDEF8)
  {
    cerr << "Register not found: " << symbols[symbol_counter] << " :: line " << line_counter << endl;
    assert(0);
  }

  return reg_id;
}

uint8_t Assembler::pop_shift(uint32_t &symbols_count, uint32_t &symbol_counter)
{
  uint8_t shift_ammount;

  if(symbol_counter >= symbols_count)
  {
    assert(0);
    return UNDEF8;
  }

  symbol_counter++;

  shift_ammount = static_cast<uint8_t>(stoi(symbols[symbol_counter]));

  return shift_ammount;
}

uint32_t Assembler::pop_jump_address(uint32_t &symbols_count, uint32_t &symbol_counter,
                                     uint32_t &label_counter)
{
  uint32_t jump_address = 0;
  string bin;

  if(symbol_counter >= symbols_count)
  {
      assert(0);
      return UNDEF32;
  }

  symbol_counter++;

  /* search the labels vector for the jump label */
  for(uint32_t i = 0; i < label_counter; i++)
  {
    if(label[i].name == symbols[symbol_counter])
    {
      jump_address = label[i].address;
      break;
    }
  }

  jump_address = ((MEM_TEXT_START & 0x0FFFFFFF)>>2) + jump_address;

  return jump_address;
}

/* load the offsets for memory instructions */
uint16_t Assembler::pop_mem_offset(uint32_t &symbols_count, uint32_t &symbol_counter)
{
  uint16_t load_offset;

  if(symbol_counter >= symbols_count)
  {
    assert(0);
    return UNDEF16;
  }

  symbol_counter++;

  load_offset = static_cast<uint16_t>(stoi(symbols[symbol_counter]));

  return load_offset;
}

/* build instruction out of symbols */
uint32_t Assembler::build_instruction(uint32_t &addr,
                                      uint32_t &symbols_count, uint32_t &symbol_counter,
                                      uint32_t &label_counter, uint32_t &line_counter)
{
  instruction_t instruction = {};
  uint32_t temp;

  if(symbol_counter >= symbols_count)
  {
    assert(0);
    return UNDEF32;
  }

  if (symbols[symbol_counter] != "nop")
  {
    size_t instruction_index = Utils::find_instruction_by_name(symbols[symbol_counter]);
    if (instruction_index == UNDEF32)
    {
      /* ignore labels as they were already recorded  */
      symbol_counter++;
      temp = build_instruction(addr, symbols_count, symbol_counter,
                               label_counter, line_counter);
      return temp;
    }
    else
    {
      instruction_format_t instruction_def = instructions_def[instruction_index];

      instruction.opcode = static_cast<uint8_t>(instruction_def.opcode);
      if (instruction_def.format == FORMAT_R)
      {
        instruction.funct = static_cast<uint8_t>(instruction_def.subopcode);
        if (instruction_def.subopcode != SUBOP_SYSCALL)
        {
          if (instruction_def.subopcode == SUBOP_JR || instruction_def.subopcode == SUBOP_JALR)
          {
            instruction.rs = pop_register(symbols_count, symbol_counter, line_counter);
          }
          else if (instruction_def.subopcode == SUBOP_SLL || instruction_def.subopcode == SUBOP_SRL)
          {
            instruction.rd = pop_register(symbols_count, symbol_counter, line_counter);
            instruction.rs = 0;
            instruction.rt = pop_register(symbols_count, symbol_counter, line_counter);
            instruction.shamt = pop_shift(symbols_count, symbol_counter);
          }
          else
          {
            instruction.rd = pop_register(symbols_count, symbol_counter, line_counter);
            instruction.rs = pop_register(symbols_count, symbol_counter, line_counter);
            instruction.rt = pop_register(symbols_count, symbol_counter, line_counter);
          }
        }
      }
      else if (instruction_def.format == FORMAT_I)
      {
        if (instruction.opcode == OP_LW || instruction.opcode == OP_SW ||
            instruction.opcode == OP_LB || instruction.opcode == OP_SB)
        {
          instruction.rt = pop_register(symbols_count, symbol_counter, line_counter);
          instruction.addr_i = pop_mem_offset(symbols_count, symbol_counter);
          instruction.rs = pop_register(symbols_count, symbol_counter, line_counter);
        }
        else if (instruction.opcode == OP_BEQ || instruction.opcode == OP_BNE)
        {
          instruction.rs = pop_register(symbols_count, symbol_counter, line_counter);
          instruction.rt = pop_register(symbols_count, symbol_counter, line_counter);
          instruction.addr_i = pop_branch_offset(symbols_count, symbol_counter, label_counter, line_counter);
        }
        else if (instruction.opcode == OP_LUI)
        {
          instruction.rt = pop_register(symbols_count, symbol_counter, line_counter);
          instruction.rs = 0;
          instruction.addr_i = pop_immediate(symbols_count, symbol_counter);
        }
        else
        {
          instruction.rt = pop_register(symbols_count, symbol_counter, line_counter);
          instruction.rs = pop_register(symbols_count, symbol_counter, line_counter);
          instruction.addr_i = pop_immediate(symbols_count, symbol_counter);
        }
      }
      else if (instruction_def.format == FORMAT_J)
      {
        instruction.addr_j = pop_jump_address(symbols_count, symbol_counter, label_counter);
      }
    }
  }

  symbol_counter++;
  line_counter++;
  temp = Utils::encode_instruction(instruction);
  return temp;
}

/* first pass through the symbol list and records labels and their addresses */
void Assembler::first_pass(uint32_t &symbols_count, uint32_t &symbolsCounter,
                          uint32_t &line_counter, uint32_t &label_counter) //This function looks through all symbols to find labels.
{
  string temp_symbol;
  size_t instruction_index;

  if(symbolsCounter >= symbols_count)
    return;

  temp_symbol = symbols[symbolsCounter];

  instruction_index = Utils::find_instruction_by_name(temp_symbol);
  if (instruction_index != UNDEF32)
  {
    symbolsCounter += instructions_def[instruction_index].symbols_count;
  }
  else
  {
    /* if it is not an instruction, assume it is a label */
    label.push_back({temp_symbol, line_counter});

    /* we do not increment the line counter */
    label_counter++;
    symbolsCounter++;
    return;
  }

  line_counter++; //We increment the line counter once after each instruction is run through. Except for when a label is found!
}

/* consume identifiers (instructions/tags) */
void Assembler::consume_identifier(char &value, uint32_t &i)
{
  string temp;

  while(isalpha(value) || isdigit(value))
  {
    if(i >= token_size)
      break;

    temp += value;
    i++;
    value = tokens[i];
  }
  symbols.push_back(temp);
}

/* Consume registers (start with $) */
void Assembler::consume_register(char &value, uint32_t &i)
{
  string temp;
  temp += value;
  i++;
  value = tokens[i];

  while(value != ',' && value != ' ' && value != ')' && value != '\n')
  {
    if(i >= token_size)
      break;

    temp += value;
    i++;
    value = tokens[i];
  }
  symbols.push_back(temp);
}

/* Consume a numeric symbol in hex or decimal format */
void Assembler::consume_digit(char &value, uint32_t &i)
{
  string temp;
  stringstream tempss;
  bool decimal = true;

  if(value == '0' && tokens[i+1] == 'x')
  {
    decimal = false;
    i += 2; // skip 0x
  }

  if(value == '-')
  {
    temp = value; // consume -
    i += 1;
  }

  value = tokens[i];

  while(isdigit(value) || isalpha(value))
  {
    if(i >= token_size)
      break;

    temp += value;
    i++;
    value = tokens[i];
  }

  if (decimal)
  {
    tempss << setw(4) << setfill('0') << hex << stoi(temp);
    temp = tempss.str();
  }
  symbols.push_back(temp);
}

} /* namespace */
