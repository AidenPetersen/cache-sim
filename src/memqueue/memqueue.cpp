#include "memqueue/memqueue.hpp"

MemQueue::MemQueue(std::string &filename) {
  file.open(filename);
  if (!file.is_open()) {
    std::cerr << "Failed to open file " << filename << std::endl;
    exit(1);
  }
}

MemOp MemQueue::next_access() {
  MemOp result;
  std::string type;
  std::string addr;
  std::string size;
  file >> type;

  if (type[0] == 'L') {
    result.type = LOAD;
  } else if (type[0] == 'S') {
    result.type = STORE;
  } else {
    // Presuming file is complete
    result.type = DONE;
    return result;
  }

  file >> addr >> size;
  // Parse in hex
  result.addr = std::stoull(addr, 0, 16);
  // Parse in dec
  result.size = std::stoull(size);
  return result;
}
