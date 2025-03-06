#include "config.hpp"
#include "memqueue/memqueue.hpp"
#include "utils.hpp"
#include <iostream>

void print_stat_data(std::shared_ptr<StatDataMap> data){
  for(auto const& [key, val] : *data){
    std::cout << key << ": ";
    if(val.sdt == INT){
      std::cout << val.i << std::endl;
    } else if (val.sdt == FLOAT){
      std::cout << val.f << std::endl;
    }
  }
}


int main(int argc, char *argv[]) {
  if (argc < 2) {
    std::cerr << "No trace path given\n";
    exit(1);
  }
  std::string input_trace_path(argv[1]);
  MemQueue mq(input_trace_path);
  Config cache = Config();

  MemOp op = mq.next_access();
  int counter = 0;
  while(op.type != DONE) {
    MemRequest req = op2req(op.type);
    cache.request(req, op.addr, op.size);
    counter++;
    if(counter > 100000) break;
  }
  std::shared_ptr<StatDataMap> stats = cache.get_stats();
  print_stat_data(stats);

}
