#ifndef CACHE_STATS_H_
#define CACHE_STATS_H_

#include <map>
#include <memory>
#include <string>
#include <iostream>

#include "structures/structures.hpp"
#include "structures/replacement.hpp"

typedef enum { STRING, FLOAT, INT } StatDataTag;


struct StatData {
public:
  StatDataTag sdt;
  union {
    //std::string s;
    int64_t i;
    double f;
  };

    StatData(){}

  explicit StatData(int64_t val) {
    sdt = INT;
    i = val;
  }

  //StatData(std::string val) {
  //  sdt = STRING;
  //  s = val;
  //}

  explicit StatData(double val) {
    sdt = FLOAT;
    f = val;
  }
};

typedef std::map<std::string, StatData> StatDataMap;

#endif // CACHE_STATS_H_
