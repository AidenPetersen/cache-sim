#include "config.hpp"
#include "memqueue/memqueue.hpp"
#include "utils.hpp"
#include <iostream>

int main(int argc, char *argv[]) {
  if (argc < 2) {
    std::cerr << "No trace path given\n";
    exit(1);
  }
  std::string input_trace_path(argv[1]);
  MemQueue mq(input_trace_path);
  Config cache = Config();

  MemOp op = mq.next_access();
  while(op.type != DONE) {
    MemRequest req = op2req(op.type);

    cache.request(req, op.addr, op.size);
    break;
  }
  cache.get_stats();

}
