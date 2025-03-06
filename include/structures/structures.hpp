#ifndef STRUCTURES_H_
#define STRUCTURES_H_

#include <algorithm>
#include <array>
#include <cstdint>

#include "structures/replacement.hpp"
#include "utils.hpp"


class ICacheStructure {
public:
  virtual ~ICacheStructure();

  virtual bool read(uint64_t addr, uint64_t size);
  virtual bool write(uint64_t addr, uint64_t size);
};

template <int block_size, int ways, int addr_exp, typename RP>
class StandardCacheStructure : public ICacheStructure {
private:
  std::array<RP, EXPTOSIZE(addr_exp)> sets;

public:
  StandardCacheStructure() {
    static_assert(std::is_base_of<IReplacementPolicy, RP>::value,
                  "RP must be derived from IReplacementPolicy");
    std::fill(sets.begin(), sets.end(), RP(ways));
  }

    virtual bool read(uint64_t addr, uint64_t size){
      uint64_t index = (addr >> (block_size - 1));
      int tag_exp = 64 - block_size - addr_exp;
      // clear tag
      index = (index << tag_exp) >> tag_exp;

      RP rp = sets[index];
      RPQueryResponse rq = rp.query_read(addr);
      return rp.hit;

    };
    virtual bool write(uint64_t addr, uint64_t size){
      uint64_t index = (addr >> (block_size - 1));
      int tag_exp = 64 - block_size - addr_exp;
      // clear tag
      index = (index << tag_exp) >> tag_exp;

      RP rp = sets[index];
      RPQueryResponse rq = rp.query_write(addr);
      return rp.hit;
    };

};

#endif // STRUCTURES_H_
