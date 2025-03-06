#ifndef MEMQUEUE_H_
#define MEMQUEUE_H_

#include <iostream>
#include <cstdint>
#include <fstream>
#include <string>

enum QueueOpType { LOAD, STORE, DONE };

typedef struct {
  QueueOpType type;
  uint64_t addr;
  uint64_t size;
} MemOp;

class MemQueue {
private:
  std::ifstream file;

public:
  MemQueue(std::string &file);
  MemOp next_access();
};

#endif // MEMQUEUE_H_
