#ifndef STRUCTURES_H_
#define STRUCTURES_H_

#include <array>
#include <concepts>
#include <cstdint>

#include "replacement.hpp"

class ICacheStructure {
public:
  ~ICacheStructure();

  virtual bool read(uint64_t addr, uint64_t size);
  virtual bool write(uint64_t addr, uint64_t size);
};

template <int block_size> class StandardCacheLine {
private:
  bool invalid;
  uint64_t tag;

public:
  bool hit(int64_t addr) {
    if (invalid) {
      return false;
    }
    return tag >> block_size == addr >> block_size;
  }
  void invalidate() { invalid = true; }
  void update(int64_t addr) {
    invalid = false;
    tag = addr >> block_size;
  }
};

template <int block_size, int ways, typename RP>
  requires std::derived_from<RP, IReplacementPolicy>
class StandardCacheSet {
private:
  std::array<StandardCacheLine<block_size>, ways> lines;
  IReplacementPolicy rp;

public:
  StandardCacheSet() {

    for (int i = 0; i < ways; i++) {
      lines[i] = StandardCacheLine<block_size>();
      rp = RP();
    }
  }
};

template <int block_size, int ways, int addr>
class StandardCacheStructure : public ICacheStructure {

public:
};

#endif // STRUCTURES_H_
