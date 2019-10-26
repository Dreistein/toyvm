cmake_minimum_required(VERSION 3.10)
project(ToyVM)

set(CMAKE_CXX_STANDARD 17)

# set up includes and source files
include_directories(vm/include)
FILE(GLOB COMMON_SOURCES vm/src/*.cpp)

# debug vm
add_executable(debugger ${COMMON_SOURCES})
target_compile_definitions(debugger PUBLIC VM_DEBUG)

# vm
add_executable(vm ${COMMON_SOURCES})
