#ifndef CONFIG_H_
#define CONFIG_H_
#include "cache_stats.hpp"
#include <memory>

typedef enum { MEM_READ, MEM_WRITE } MemRequest;

// Set methods in config.cpp for your cache implementation
class Config {

private:
  class Impl;
  std::unique_ptr<Impl> pimpl;

public:
  Config();
  ~Config();
  void request(MemRequest req, uint64_t addr, uint64_t size, int core = 0);
  StatDataMap get_stats();
};

#endif // CONFIG_H_
