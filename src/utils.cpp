#include "utils.hpp"

MemRequest op2req(QueueOpType op) {
  if (op == LOAD) {
    return MEM_READ;
  } else if (op == STORE) {
    return MEM_WRITE;
  } else {
    std::cerr << "Invalid value given to op2req" << std::endl;
    exit(1);
  }
}
