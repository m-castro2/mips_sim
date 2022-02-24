#include "mem.h"
#include <stdio.h>
#include <cassert>
#include <iostream>
#include <iomanip>

namespace mips_sim
{

using namespace std;

Memory::Memory( void )
{
  /* initialize memory */
  for (size_t i = 0; i < MEM_NREGIONS; i++) {
    MEM_REGIONS[i].mem = static_cast<uint8_t *>(malloc(MEM_REGIONS[i].size));
    memset(MEM_REGIONS[i].mem, 0, MEM_REGIONS[i].size);
  }

  //TODO: Initialize memory
  for (size_t i = 0; i < 20; i++) {
    mem_write_32(static_cast<uint32_t>(MEM_DATA_START + i*4), static_cast<uint32_t>(i+1));
  }
}

Memory::~Memory()
{
  /* initialize memory */
  for (size_t i = 0; i < MEM_NREGIONS; i++) {
    if (MEM_REGIONS[i].mem)
    {
      free(MEM_REGIONS[i].mem);
      MEM_REGIONS[i].mem = nullptr;
    }
  }
}

uint32_t Memory::mem_read_32(uint32_t address) const
{
    for (size_t i = 0; i < MEM_NREGIONS; i++)
    {
        if (address >= MEM_REGIONS[i].start &&
                address < (MEM_REGIONS[i].start + MEM_REGIONS[i].size))
        {
            uint32_t offset = address - MEM_REGIONS[i].start;

            uint32_t v = static_cast<uint32_t>
               ((MEM_REGIONS[i].mem[offset+3] << 24) |
                (MEM_REGIONS[i].mem[offset+2] << 16) |
                (MEM_REGIONS[i].mem[offset+1] <<  8) |
                (MEM_REGIONS[i].mem[offset+0] <<  0));

            return v;
        }
    }

    assert(0);
    return 0;
}

void Memory::mem_write_32(uint32_t address, uint32_t value)
{
    for (size_t i = 0; i < MEM_NREGIONS; i++)
    {
        if (address >= MEM_REGIONS[i].start &&
                address < (MEM_REGIONS[i].start + MEM_REGIONS[i].size))
        {
            uint32_t offset = address - MEM_REGIONS[i].start;

            MEM_REGIONS[i].mem[offset+3] = (value >> 24) & 0xFF;
            MEM_REGIONS[i].mem[offset+2] = (value >> 16) & 0xFF;
            MEM_REGIONS[i].mem[offset+1] = (value >>  8) & 0xFF;
            MEM_REGIONS[i].mem[offset+0] = (value >>  0) & 0xFF;

            return;
        }
    }

    assert(0);
}

void Memory::print_memory( uint32_t start, uint32_t length ) const
{
  for (size_t i = 0; i < MEM_NREGIONS; i++)
  if (start >= MEM_REGIONS[i].start &&
      start+length < (MEM_REGIONS[i].start + MEM_REGIONS[i].size))
  {
    cout << endl;
    for (uint32_t mem_addr=start; mem_addr<start+length; mem_addr+=16)
    {
      uint32_t word = mem_read_32(mem_addr);
      cout << setw(8) << setfill(' ') << hex << mem_addr << " [" << setw(8) << hex << word << "]";
      word = mem_read_32(mem_addr + 4);
      cout << " [" << setw(8) << hex << word << "]";
      word = mem_read_32(mem_addr + 8);
      cout << " [" << setw(8) << hex << word << "]";
      word = mem_read_32(mem_addr + 12);
      cout << " [" << setw(8) << hex << word << "]" << endl;
    }
    return;
  }

  assert(0);
}

} /* namespace */
