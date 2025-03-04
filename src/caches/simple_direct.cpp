#include "caches/cache.hpp"
#include <map>
#include <memory>
#include <string>

class SimpleDirect : public ICache {
private:
  std::shared_ptr<std::map<std::string, std::string>> stats;

public:
  SimpleDirect
}
