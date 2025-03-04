#ifndef MEMQUEUE_H_
#define MEMQUEUE_H_

#include<cstdint>
#include<string>
#include<fstream>

enum MemOpType {LOAD, STORE};

typedef struct {
    MemOpType type;
    uint64_t addr;
    uint64_t size;
} MemOp;

class MemQueue {
    private:
        std::ifstream file;

    public:
        MemQueue(std::string& file);
        MemOp next_access();
};


#endif // MEMQUEUE_H_
