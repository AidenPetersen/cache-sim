#ifndef UTILS_H_
#define UTILS_H_

#include "config.hpp"
#include "memqueue/memqueue.hpp"

#define EXPTOSIZE(exp) (1 << (exp - 1))

MemRequest op2req(QueueOpType op);

#endif // UTILS_H_
