cmake_minimum_required(VERSION 3.5)
project(cache-sim)

# executable files
file(GLOB_RECURSE sources  src/*.cpp)
# library headers
file(GLOB_RECURSE include  include/*.hpp)
# library header implementations
file(GLOB_RECURSE lib      lib/*.cpp)

add_executable(cache-sim ${sources} ${lib})

target_compile_options(cache-sim PUBLIC -std=c++17 -Wall)

target_include_directories(cache-sim PUBLIC include)
