#ifndef REPLACEMENT_H_
#define REPLACEMENT_H_

#include <cstdint>
#include <cstdlib>
class IReplacementPolicy {
public:
  // One transaction for an address in a set:
  void inform(uint64_t addr);
  uint64_t query(uint64_t addr);
};

template <int ways> class RandomReplacement : public IReplacementPolicy {
  RandomReplacement() { srand(0); }

  void inform([[maybe_unused]] uint64_t addr) { return; }
  uint64_t query([[maybe_unused]] uint64_t addr) { return rand() % ways; }
};

#endif // REPLACEMENT_H_
