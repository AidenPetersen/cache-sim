#ifndef REPLACEMENT_H_
#define REPLACEMENT_H_

#include <cstdint>
#include <cstdlib>
#include <vector>

#include "structures/cache.hpp"

typedef struct {
  int way;
  bool evicted;
  bool hit;
} RPQueryResponse;

typedef struct {
  bool invalid;
  uint64_t addr;
} RPSetData;

// Handles cache set and replacement
class [[maybe_unused]] IReplacementPolicy {
public:
  virtual RPQueryResponse query_read(uint64_t addr) = 0;
  virtual RPQueryResponse query_write(uint64_t addr) = 0;

  virtual bool peek(uint64_t addr) = 0;

  virtual ~IReplacementPolicy() {}
};

class RandomReplacement : public IReplacementPolicy {
private:
  std::vector<RPSetData> set_data;
  int block_size;

public:
  RandomReplacement() : block_size(1) { srand(0); }
  RandomReplacement(int ways, [[maybe_unused]] int block_size)
      : block_size(block_size) {
    set_data = std::vector<RPSetData>(ways);
    RPSetData invalid;
    invalid.invalid = true;
    std::fill(set_data.begin(), set_data.end(), invalid);
    srand(0);
  }

  // Read/Write query w/ addr. Updates state and returns if hit or not.
  RPQueryResponse gen_query(uint64_t addr) {
    // Make addr ignore block
    addr = addr >> block_size;

    int ways = set_data.size();
    RPQueryResponse qs;
    qs.evicted = false;
    // Check for hit
    for (int i = 0; i < ways; i++) {
      if (!set_data[i].invalid && set_data[i].addr == addr) {
        qs.way = i;
        qs.hit = true;
        return qs;
      }
    }
    // Check for invalid entry
    for (int i = 0; i < ways; i++) {
      if (set_data[i].invalid) {
        // Update state
        set_data[i].invalid = false;
        set_data[i].addr = addr;
        qs.way = i;
        qs.hit = false;
        return qs;
      }
    }
    // Choose random entry and evict
    qs.way = rand() % ways;
    qs.hit = false;
    qs.evicted = true;

    set_data[qs.way].invalid = false;
    set_data[qs.way].addr = addr;
    return qs;
  }

  bool peek(uint64_t addr) {
    addr = addr >> block_size;
    int ways = set_data.size();
    // Check for hit
    for (int i = 0; i < ways; i++) {
      if (!set_data[i].invalid && set_data[i].addr == addr) {
        return true;
      }
    }
    return false;
  }

  virtual RPQueryResponse query_read(uint64_t addr) { return gen_query(addr); }

  virtual RPQueryResponse query_write(uint64_t addr) { return gen_query(addr); }
};

class LRUReplacement : public IReplacementPolicy {
private:
  std::vector<RPSetData> set_data;
  std::vector<int> last_used;
  int block_size;

  int find_lru_index() {
    // max of last used
    int index_max = -1;
    int max = -1;
    for (int i = 0; i < (int)last_used.size(); i++) {
      if (last_used[i] > max) {
        max = last_used[i];
        index_max = i;
      }
    }
    return index_max;
  }
  void update_usage(int way) {

    for (int i = 0; i < (int)last_used.size(); i++) {
      if (way == i) {
        last_used[i] = 0;
        continue;
      }
      last_used[i]++;
    }
  }

public:
  LRUReplacement() : block_size(1) { srand(0); }
  LRUReplacement(int ways, [[maybe_unused]] int block_size)
      : block_size(block_size) {
    set_data = std::vector<RPSetData>(ways);
    last_used = std::vector<int>(ways);
    RPSetData invalid;
    invalid.invalid = true;
    std::fill(set_data.begin(), set_data.end(), invalid);
    std::fill(last_used.begin(), last_used.end(), 0);
    srand(0);
  }

  // Read/Write query w/ addr. Updates state and returns if hit or not.
  RPQueryResponse gen_query(uint64_t addr) {
    // Make addr ignore block
    addr = addr >> block_size;

    int ways = set_data.size();
    RPQueryResponse qs;
    qs.evicted = false;
    // Check for hit
    for (int i = 0; i < ways; i++) {
      if (!set_data[i].invalid && set_data[i].addr == addr) {
        qs.way = i;
        qs.hit = true;
        update_usage(i);
        return qs;
      }
    }
    // Check for invalid entry
    for (int i = 0; i < ways; i++) {
      if (set_data[i].invalid) {
        // Update state
        set_data[i].invalid = false;
        set_data[i].addr = addr;
        qs.way = i;
        qs.hit = false;
        update_usage(i);
        return qs;
      }
    }
    // Choose choose lru way to evict
    qs.way = find_lru_index();
    qs.hit = false;
    qs.evicted = true;

    set_data[qs.way].invalid = false;
    set_data[qs.way].addr = addr;
    return qs;
  }

  bool peek(uint64_t addr) {
    addr = addr >> block_size;
    int ways = set_data.size();
    // Check for hit
    for (int i = 0; i < ways; i++) {
      if (!set_data[i].invalid && set_data[i].addr == addr) {
        return true;
      }
    }
    return false;
  }

  virtual RPQueryResponse query_read(uint64_t addr) { return gen_query(addr); }

  virtual RPQueryResponse query_write(uint64_t addr) { return gen_query(addr); }
};
#endif // REPLACEMENT_H_
