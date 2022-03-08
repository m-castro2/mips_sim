#include "mem.h"
#include "utils.h"
#include "exception.h"

#include <stdio.h>
#include <cassert>
#include <sstream>
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
          stringstream ss;
          ss << "Memory allocation beyond the region limit: Address "
             << Utils::hex32(address) << " / size " << size << " Bytes";
          throw Exception::e(MEMORY_ALLOC_EXCEPTION, ss.str());
        }
        return;
      }
    }
    throw Exception::e(MEMORY_ALLOC_EXCEPTION,
                       "Invalid memory space allocation",
                        address);
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
    throw Exception::e(MEMORY_READ_EXCEPTION,
                       "Invalid read memory address",
                        address);
  }

  throw Exception::e(MEMORY_READ_EXCEPTION,
                     "Read to invalid memory space",
                      address);
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
    throw Exception::e(MEMORY_WRITE_EXCEPTION,
                       "Invalid write memory addresss",
                        address);
  }

  throw Exception::e(MEMORY_WRITE_EXCEPTION,
                     "Write to invalid memory space",
                      address);
}

void Memory::print_memory( uint32_t start, uint32_t length, ostream &out ) const
{
  for (size_t i = 0; i < MEM_NREGIONS; i++)
  if (start >= MEM_REGIONS[i].start &&
      start+length <= (MEM_REGIONS[i].start + MEM_REGIONS[i].size))
  {
    try
    {
      for (uint32_t mem_addr=start; mem_addr<start+length; mem_addr+=16)
      {
        uint32_t word = mem_read_32(mem_addr);
        out << setw(8) << setfill(' ') << Utils::hex32(mem_addr) << " [" << Utils::hex32(word) << "]";
        word = mem_read_32(mem_addr + 4);
        out << " [" << Utils::hex32(word) << "]";
        word = mem_read_32(mem_addr + 8);
        out << " [" << Utils::hex32(word) << "]";
        word = mem_read_32(mem_addr + 12);
        out << " [" << Utils::hex32(word) << "]" << endl;
      }
    }
    catch (int)
    {
      out << endl;
      /* ignore */
    }
    return;
  }

  stringstream ss;
  ss << "Read access to invalid memory space: Start "
     << Utils::hex32(start) << " / size " << length << " Bytes";
  throw Exception::e(MEMORY_ALLOC_EXCEPTION, ss.str());
}

} /* namespace */
