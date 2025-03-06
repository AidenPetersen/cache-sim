#ifndef STRUCTURES_H_
#define STRUCTURES_H_

#include <algorithm>
#include <array>
#include <cstdint>

#include "structures/replacement.hpp"

class ICacheStructure {
public:
  virtual ~ICacheStructure() {}

    virtual bool read(uint64_t addr, uint64_t size) = 0;
    virtual bool write(uint64_t addr, uint64_t size) = 0;
};

template <int block_size, int ways, int addr, typename RP>
class StandardCacheStructure : public ICacheStructure {
private:
  std::array<RP, addr> sets;

public:
  StandardCacheStructure() {
    static_assert(std::is_base_of<IReplacementPolicy, RP>::value,
                  "RP must be derived from IReplacementPolicy");
    std::fill(sets.begin(), sets.end(), RP(ways, block_size));
  }

  virtual bool read(uint64_t address, uint64_t size) {
    uint64_t index = (address >> (block_size - 1));
    int tag_exp = 64 - block_size - addr;
    // clear tag
    index = (index << tag_exp) >> tag_exp;

    RP rp = sets[index];
    RPQueryResponse rq = rp.query_read(addr);
    return rq.hit;
  };
  virtual bool write(uint64_t address, uint64_t size) {
    uint64_t index = (addr >> (block_size - 1));
    int tag_exp = 64 - block_size - addr;
    // clear tag
    index = (index << tag_exp) >> tag_exp;

    RP rp = sets[index];
    RPQueryResponse rq = rp.query_write(addr);
    return rq.hit;
  };
};

#endif // STRUCTURES_H_
