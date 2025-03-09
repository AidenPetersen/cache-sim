#ifndef STRUCTURES_H_
#define STRUCTURES_H_

#include <algorithm>
#include <array>
#include <cmath>
#include <cstdint>
#include <cstdio>
#include <iostream>

#include "structures/replacement.hpp"

// Contains information from request
// relevant for statistic monitoring
struct StructureReadResp {
  bool hit;
  bool evicted;
};

struct StructureWriteResp {
  bool hit;
  bool evicted;
};

class [[maybe_unused]] ICacheStructure {
public:
  virtual ~ICacheStructure() {}

  virtual StructureReadResp read(uint64_t addr,
                                 [[maybe_unused]] uint64_t size) = 0;
  virtual StructureWriteResp write(uint64_t addr,
                                   [[maybe_unused]] uint64_t size) = 0;

  // Loads int data at address without transaction. Used for prefetch
  virtual void put(uint64_t addr) = 0;

  // Checks if address is in cache without updating
  virtual bool peek(uint64_t addr) = 0;

};

template <int block_size, int ways, int addr, typename RP>
class StandardCacheStructure : public ICacheStructure {
private:
  std::array<RP, addr> sets;
  uint64_t index_mask;

  void set_index_mask() {
    int index_bits = 0;
    int index = addr;
    while (index >>= 1)
      ++index_bits;

    int mask_start = block_size;
    int mask_end = mask_start + index_bits;
    index_mask = 0;
    for (int i = mask_start; i < mask_end; i++) {
      index_mask |= (1 << i);
    }
  }

public:
  StandardCacheStructure() {
    static_assert(std::is_base_of<IReplacementPolicy, RP>::value,
                  "RP must be derived from IReplacementPolicy");
    std::fill(sets.begin(), sets.end(), RP(ways, block_size));
    set_index_mask();
  }

  virtual StructureReadResp read(uint64_t address,
                                 [[maybe_unused]] uint64_t size) {
    uint64_t index = ((address & index_mask) >> block_size);

    RP *rp = &sets[index];
    RPQueryResponse rq = rp->query_read(address >> block_size);

    StructureReadResp resp;
    resp.evicted = rq.evicted;
    resp.hit = rq.hit;
    return resp;
  };
  virtual StructureWriteResp write(uint64_t address,
                                   [[maybe_unused]] uint64_t size) {
    uint64_t index = ((address & index_mask) >> block_size);

    RP *rp = &sets[index];
    RPQueryResponse rq = rp->query_read(address >> block_size);
    StructureWriteResp resp;
    resp.evicted = rq.evicted;
    resp.hit = rq.hit;
    return resp;
  };

  virtual void put(uint64_t address) {
    uint64_t index = ((address & index_mask) >> block_size);
    RP *rp = &sets[index];
    rp->query_read(address >> block_size);
  }

  virtual bool peek(uint64_t address) {
    uint64_t index = ((address & index_mask) >> block_size);
    RP *rp = &sets[index];
    return rp->peek(address >> block_size);
  }
};

#endif // STRUCTURES_H_
