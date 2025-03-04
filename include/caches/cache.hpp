#ifndef CACHE_H_
#define CACHE_H_

#include "cache_stats.hpp"
#include <cstdint>

typedef enum { MEM_READ, MEM_WRITE } MemRequest;

class ICache {
public:
  virtual ~ICache();
  virtual void request(MemRequest req, uint64_t addr, uint64_t type,
                       int core = 0);
  virtual StatDataMap get_stats();

#endif // CACHE_H_
