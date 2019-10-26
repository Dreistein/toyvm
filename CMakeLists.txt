cmake_minimum_required(VERSION 3.10)
project(ToyVM)

set(CMAKE_CXX_STANDARD 17)

#set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/vm/out/)
#set(CMAKE_EXE_LINKER_FLAGS "-static-libgcc -static-libstdc++ -static -mthreads")

include_directories(vm/include)
set(COMMON_SOURCES  vm/main.cpp vm/src/Instruction.cpp vm/src/VM.cpp)

add_executable(debugger vm/src/Debugger.cpp ${COMMON_SOURCES})
target_compile_definitions(debugger PUBLIC VM_DEBUG)

#add_executable(vm ${COMMON_SOURCES})
