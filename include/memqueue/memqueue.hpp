#ifndef MEMQUEUE_H_
#define MEMQUEUE_H_

#include <cstdint>
#include <fstream>
#include <iostream>
#include <string>

enum QueueOpType { LOAD, STORE, DONE };

struct MemOp {
  MemOp(QueueOpType type, uint64_t addr, uint64_t size)
      : type(type), addr(addr), size(size) {}
  MemOp() : type(DONE), addr(0), size(0) {}
  QueueOpType type;
  uint64_t addr;
  uint64_t size;
};

class MemQueue {
private:
  std::ifstream file;

public:
  MemQueue(std::string &file);
  MemOp next_access();
};

#endif // MEMQUEUE_H_
