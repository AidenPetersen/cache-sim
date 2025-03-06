#ifndef CACHE_STATS_H_
#define CACHE_STATS_H_

#include <map>
#include <memory>
#include <string>

typedef enum { STRING, FLOAT, INT } StatDataTag;

typedef struct {
  StatDataTag sdt;
  union {
    std::string s;
    uint64_t i;
    double f;
  };
} StatData;

typedef std::shared_ptr<std::map<std::string, StatData>> StatDataMap;

#endif // CACHE_STATS_H_
