#include "memqueue.hpp"
#include <iostream>

int main(int argc, char *argv[]) {
  std::string input_trace_path(argv[1]);
  MemQueue mq(input_trace_path);
}
