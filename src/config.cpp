#include "config.hpp"
#include "structures/cache.hpp"
#include "structures/prefetcher.hpp"
#include <iostream>
#include <cstdio>

#define L1_BLOCK_SIZE 6
#define L1_WAYS 2
#define L1_INDEX 1024
#define L1_REPLACEMENT_POLICY RandomReplacement
#define L1_HIT_CYCLES 3

#define L2_BLOCK_SIZE 6
#define L2_WAYS 8
#define L2_INDEX 4096
#define L2_REPLACEMENT_POLICY RandomReplacement
#define L2_HIT_CYCLES 8

#define ENABLE_PREFETCH false
#define BLOCKING_PREFETCH false
#define PREFETCHER OBLPrefetcher

#define L1_CACHE_TYPE                                                          \
  StandardCacheStructure<L1_BLOCK_SIZE, L1_WAYS, L1_INDEX,                     \
                         L1_REPLACEMENT_POLICY>
#define L2_CACHE_TYPE                                                          \
  StandardCacheStructure<L2_BLOCK_SIZE, L2_WAYS, L2_INDEX,                     \
                         L2_REPLACEMENT_POLICY>

#define MEMORY_CYCLES 200

class Config::Impl {

private:
  L1_CACHE_TYPE l1;
  L2_CACHE_TYPE l2;

  PREFETCHER prefetcher;
  std::shared_ptr<StatDataMap> stats;
  uint64_t cycle = 0;
  bool prefetch_busy = false;
  uint64_t prefetch_busy_cycles = 0;

  bool l1_cache_req(uint64_t address, uint64_t size, MemRequest req) {

    if (req == MEM_READ) {
      StructureReadResp resp = l1.read(address, size);

      if (resp.hit) {
        (*stats)["l1_hits"].i += 1;
      } else {
        (*stats)["l1_evic"].i += resp.evicted ? 1 : 0;
        (*stats)["l1_miss"].i += 1;
      }

      return resp.hit;
    } else {
      StructureWriteResp resp = l1.write(address, size);
      if (resp.hit) {
        (*stats)["l1_hits"].i += 1;
      } else {

        (*stats)["l1_evic"].i += resp.evicted ? 1 : 0;
        (*stats)["l1_miss"].i += 1;
      }
      return resp.hit;
    }
  }

  bool l2_cache_req(uint64_t address, uint64_t size, MemRequest req) {

    if (req == MEM_READ) {
      StructureReadResp resp = l2.read(address, size);

      if (resp.hit) {
        (*stats)["l2_hits"].i += 1;
      } else {

        (*stats)["l2_evic"].i += resp.evicted ? 1 : 0;
        (*stats)["l2_miss"].i += 1;
      }
      return resp.hit;
    } else {
      StructureWriteResp resp = l2.write(address, size);
      if (resp.hit) {
        (*stats)["l2_hits"].i += 1;
      } else {
        (*stats)["l2_evic"].i += resp.evicted ? 1 : 0;
        (*stats)["l2_miss"].i += 1;
      }
      return resp.hit;
    }
  }

  void update_cycles(uint64_t op_duration) {
    cycle += op_duration;
    if (prefetch_busy) {
      if (op_duration >= prefetch_busy_cycles) {
        prefetch_busy_cycles = 0;
        prefetch_busy = false;
        prefetcher.prefetch_complete();
        (*stats)["num_prefetches"].i += 1;
      } else {
        prefetch_busy_cycles -= op_duration;
      }
    }
  }

  void wait_for_prefetch() {
    cycle += prefetch_busy_cycles;
    (*stats)["prefetch_wait_cycles"].i += prefetch_busy_cycles;

    prefetch_busy_cycles = 0;
    prefetch_busy = false;
    prefetcher.prefetch_complete();

    (*stats)["num_prefetches"].i += 1;
  }

public:
  Impl()
      : prefetcher(std::make_shared<L2_CACHE_TYPE>(l2), L2_BLOCK_SIZE),
        stats(std::make_shared<StatDataMap>()) {

    (*stats)["l1_hits"] = StatData(static_cast<int64_t>(0));
    (*stats)["l1_miss"] = StatData(static_cast<int64_t>(0));
    (*stats)["l1_evic"] = StatData(static_cast<int64_t>(0));
    (*stats)["l1_hit_ratio"] = StatData(static_cast<double>(0));

    (*stats)["l2_hits"] = StatData(static_cast<int64_t>(0));
    (*stats)["l2_miss"] = StatData(static_cast<int64_t>(0));
    (*stats)["l2_evic"] = StatData(static_cast<int64_t>(0));
    (*stats)["l2_hit_ratio"] = StatData(static_cast<double>(0));

    (*stats)["cycles"] = StatData(static_cast<int64_t>(0));

    if (ENABLE_PREFETCH) {
      if (BLOCKING_PREFETCH) {
        (*stats)["prefetch_wait_cycles"] = StatData(static_cast<int64_t>(0));
      }
      (*stats)["num_prefetches"] = StatData(static_cast<int64_t>(0));

      (*stats)["miss_during_prefetch"] = StatData(static_cast<int64_t>(0));
    }
  }

  void request(MemRequest req, uint64_t addr, uint64_t size,
               [[maybe_unused]] int core = 0) {
    // L1 Request
    bool l1_hit = l1_cache_req(addr, size, req);
    if (l1_hit) {
      update_cycles(L1_HIT_CYCLES);

      //if (!prefetch_busy && ENABLE_PREFETCH) {
      //  prefetch_busy = prefetcher.prefetch_start_req();
      //  prefetch_busy_cycles = prefetch_busy ? MEMORY_CYCLES : 0;
      //}
    } else {
      // L2 Request
      bool l2_hit = l2_cache_req(addr, size, req);
      if (l2_hit) {
        update_cycles(L2_HIT_CYCLES);

        if (!prefetch_busy && ENABLE_PREFETCH) {
          prefetch_busy = prefetcher.prefetch_start_req();
          prefetch_busy_cycles = prefetch_busy ? MEMORY_CYCLES : 0;
        }
      } else {
        if (prefetch_busy) {
          if (BLOCKING_PREFETCH) {
            wait_for_prefetch();
          }
          // If prefetching value already
          if (prefetcher.prefetch_in_prog(addr)) {

            (*stats)["miss_during_prefetch"].i += 1;
            update_cycles(prefetch_busy_cycles);
          } else {
            update_cycles(MEMORY_CYCLES);
          }
        } else {
          update_cycles(MEMORY_CYCLES);
        }
      }

      // Tell prefetcher about result
      prefetcher.inform(addr, l2_hit);
    }
  }

  std::shared_ptr<StatDataMap> get_stats() {
    std::cout << "Getting cache stats" << std::endl;

    double l1_hit = (double)(*stats)["l1_hits"].i;
    double l1_miss = (double)(*stats)["l1_miss"].i;
    (*stats)["l1_hit_ratio"] = StatData(l1_hit / (l1_hit + l1_miss));

    double l2_hit = (double)(*stats)["l2_hits"].i;
    double l2_miss = (double)(*stats)["l2_miss"].i;
    (*stats)["l2_hit_ratio"] = StatData(l2_hit / (l2_hit + l2_miss));

    (*stats)["cycles"] = StatData(static_cast<int64_t>(cycle));

    return stats;
  }
};

Config::Config() : pimpl(std::make_unique<Impl>()) {}
Config::~Config() = default;

// Public member implementations
void Config::request(MemRequest req, uint64_t addr, uint64_t size, int core) {
  pimpl->request(req, addr, size, core);
}

std::shared_ptr<StatDataMap> Config::get_stats() { return pimpl->get_stats(); }
