#include "mem.h"
#include "utils.h"

#include <stdio.h>
#include <cassert>
#include <iostream>
#include <iomanip>

namespace mips_sim
{

using namespace std;

Memory::Memory( void )
{
  locked = false;

  /* initialize memory */
  for (size_t i = 0; i < MEM_NREGIONS; i++) {
    MEM_REGIONS[i].mem = static_cast<uint8_t *>(malloc(MEM_REGIONS[i].size));
    memset(MEM_REGIONS[i].mem, 0, MEM_REGIONS[i].size);
  }

  //Arbitrarily initialize memory
  //TODO: Parse from asm code
  uint32_t datamemwords = 100;
  allocated_regions.push_back({MEM_DATA_START, datamemwords*4, nullptr});
  for (size_t i = 0; i < datamemwords; i++) {
    mem_write_32(static_cast<uint32_t>(MEM_DATA_START + i*4), static_cast<uint32_t>(i+1));
  }
}

void Memory::snapshot(int r)
{
  if (MEM_SNAPSHOT[r].mem == nullptr)
    MEM_SNAPSHOT[r].mem = static_cast<uint8_t *>(malloc(MEM_SNAPSHOT[r].size));

  memcpy(MEM_SNAPSHOT[r].mem, MEM_REGIONS[r].mem, MEM_SNAPSHOT[r].size);
}

void Memory::reset(int r)
{
  assert(MEM_SNAPSHOT[r].mem != nullptr);

  memcpy(MEM_REGIONS[r].mem, MEM_SNAPSHOT[r].mem, MEM_SNAPSHOT[r].size);
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

    if (MEM_SNAPSHOT[i].mem)
    {
      free(MEM_SNAPSHOT[i].mem);
      MEM_SNAPSHOT[i].mem = nullptr;
    }
  }
}

void Memory::lock() { locked = true; }

void Memory::unlock() { locked = false; }

void Memory::allocate_space(uint32_t address, uint32_t size)
{
    for (size_t i = 0; i < MEM_NREGIONS; i++)
    {
      if (address >= MEM_REGIONS[i].start &&
              (address) < (MEM_REGIONS[i].start + MEM_REGIONS[i].size))
      {
        if ((address + size) < (MEM_REGIONS[i].start + MEM_REGIONS[i].size))
        {
          allocated_regions.push_back({address, size, nullptr});
        }
        else
        {
          //TODO: Space goes beyond the limit
          cerr << "Memory allocation beyond the region limit / Address: 0x"
               << Utils::hex32(address) << " Size: " << size << " Bytes" << endl;
          assert(0);
        }
        return;
      }
    }

    //TODO INVALID MEMORY SPACE
    cerr << "Invalid memory space: 0x" << Utils::hex32(address) << endl;
    assert(0);
    return;
}

uint32_t Memory::mem_read_32(uint32_t address) const
{
  bool valid_address = !locked;

  if (locked)
  {
    /* check if address is valid */
    for (mem_region_t region : allocated_regions)
    {
      if (address >= region.start &&
          address < (region.start + region.size))
      {
        valid_address = true;
        break;
      }
    }
  }

  if (valid_address)
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
  }
  else
  {
    //TODO INVALID MEMORY EXCEPTION
    cerr << "Invalid memory exception [Read]: 0x" << Utils::hex32(address) << endl;

    assert(0);
    return 0;
  }

  //TODO INVALID MEMORY ADDRESS
  cerr << "Read access to invalid memory address: 0x" << Utils::hex32(address) << endl;
  assert(0);
  return 0;
}

void Memory::mem_write_32(uint32_t address, uint32_t value)
{
  bool valid_address = !locked;

  if (locked)
  {
    /* check if address is valid */
    for (mem_region_t region : allocated_regions)
    {
      if (address >= region.start &&
          address < (region.start + region.size))
      {
        valid_address = true;
        break;
      }
    }
  }

  if (valid_address)
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
  }
  else
  {
    //TODO INVALID MEMORY EXCEPTION
    cerr << "Invalid memory exception [Write]: 0x" << Utils::hex32(address) << endl;
    assert(0);
  }

  //TODO INVALID MEMORY ADDRESS
  cerr << "Write access to invalid memory address: 0x" << Utils::hex32(address) << endl;
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
      cout << setw(8) << setfill(' ') << Utils::hex32(mem_addr) << " [" << Utils::hex32(word) << "]";
      word = mem_read_32(mem_addr + 4);
      cout << " [" << Utils::hex32(word) << "]";
      word = mem_read_32(mem_addr + 8);
      cout << " [" << Utils::hex32(word) << "]";
      word = mem_read_32(mem_addr + 12);
      cout << " [" << Utils::hex32(word) << "]" << endl;
    }
    return;
  }

  assert(0);
}

} /* namespace */
