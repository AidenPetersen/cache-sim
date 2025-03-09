#ifndef PREFETCHER_H_
#define PREFETCHER_H_

#include "structures/cache.hpp"
#include <cstdint>
#include <memory>

class [[maybe_unused]] IPrefetcher {
public:
  virtual ~IPrefetcher() {}
  virtual void inform(uint64_t addr, bool was_hit) = 0;

  // Starts background mem, returns if successfully started
  virtual bool prefetch_start_req() = 0;

  // Checks if address is being prefetched
  virtual bool prefetch_in_prog(uint64_t addr) = 0;

  // Finished background mem transaciton and loads into cache.
  virtual void prefetch_complete() = 0;
};

class OBLPrefetcher : public IPrefetcher {
private:
  std::shared_ptr<ICacheStructure> cache;
  uint64_t next_addr = 0;
  bool prev_was_hit = false;
  int block_size;
  bool is_prefetching;
  uint64_t prefetch_addr;

public:
  OBLPrefetcher(std::shared_ptr<ICacheStructure> cache, int block_size)
      : cache(cache), block_size(block_size) {}

  OBLPrefetcher() {}

  virtual void inform(uint64_t addr, bool was_hit) {
    next_addr = ((addr >> block_size) << block_size);
    prev_was_hit = was_hit;
  }

  virtual bool prefetch_start_req() {

    // Make sure prefetch address isn't already in cache
    // Only prefetch on hit
    if (!is_prefetching && prev_was_hit) {
      bool already_loaded = cache->peek(next_addr);
      if (!already_loaded) {
        is_prefetching = true;
        prefetch_addr = next_addr;
        return true;
      }
    }
    return false;
  }

  virtual bool prefetch_in_prog(uint64_t addr) {
      bool in_block = (addr >> block_size) == ((prefetch_addr >> block_size) + 1);
      return in_block && is_prefetching;
  }

  virtual void prefetch_complete() {
    uint64_t offset = 1 << (block_size - 1);
    cache->put(prefetch_addr + 1 * offset);
    cache->put(prefetch_addr + 2 * offset);
    cache->put(prefetch_addr + 3 * offset);
    cache->put(prefetch_addr + 4 * offset);
    is_prefetching = false;
  }
};

#endif // PREFETCHER_H_
