#include "config.hpp"
#include <iostream>

class Config::Impl {
    public:
        void request(MemRequest req, uint64_t addr, uint64_t size, [[maybe_unused]] int core = 0){
            std::cout << "Making request test" << std::endl;

        }

        StatDataMap get_stats(){
            std::cout << "Getting cache stats" << std::endl;
            return StatDataMap();
        }
};

Config::Config(): pimpl( std::make_unique<Impl>() ){}
Config::~Config() = default;

// Public member implementations
void Config::request(MemRequest req, uint64_t addr, uint64_t size, int core){
    pimpl->request(req, addr, size, core);
}

StatDataMap Config::get_stats(){
    return pimpl->get_stats();
}

