#include "config.hpp"
#include "structures/structures.hpp"
#include <iostream>

class Config::Impl {

private:
  StandardCacheStructure<4, 4, 1024, RandomReplacement> l1;
  std::shared_ptr<StatDataMap> stats;

public:
  Impl() : stats(std::make_shared<StatDataMap>()) {
    (*stats)["l1_hits"] = StatData(static_cast<int64_t>(0));
    (*stats)["l1_miss"] = StatData(static_cast<int64_t>(0));
    (*stats)["l1_evic"] = StatData(static_cast<int64_t>(0));
    (*stats)["l1_hit_ratio"] = StatData(static_cast<double>(0));
  }

  void request(MemRequest req, uint64_t addr, uint64_t size,
               [[maybe_unused]] int core = 0) {
    if (req == MEM_READ) {
      StructureReadResp resp = l1.read(addr, size);

      if (resp.hit) {
        (*stats)["l1_hits"].i += 1;
      } else {

        (*stats)["l1_evic"].i += resp.evicted ? 1 : 0;
        (*stats)["l1_miss"].i += 1;
      }
    } else if (req == MEM_WRITE) {
      StructureWriteResp resp = l1.write(addr, size);
      if (resp.hit) {
        (*stats)["l1_hits"].i += 1;
      } else {

        (*stats)["l1_evic"].i += resp.evicted ? 1 : 0;
        (*stats)["l1_miss"].i += 1;
      }
    }
  }

  std::shared_ptr<StatDataMap> get_stats() {
    std::cout << "Getting cache stats" << std::endl;

    double l1_hit = (double)(*stats)["l1_hits"].i;
    double l1_miss = (double)(*stats)["l1_miss"].i;
    (*stats)["l1_hit_ratio"] = StatData(l1_hit / (l1_hit + l1_miss));
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
