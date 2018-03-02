cmake_minimum_required(VERSION 3.10)
project(ToyVM)

set(CMAKE_CXX_STANDARD 17)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/vm/out/)
set(CMAKE_EXE_LINKER_FLAGS "-static-libgcc -static-libstdc++ -static -mthreads")

set(SOURCE_FILES    vm/main.cpp
                    vm/Instruction.cpp vm/Instruction.h
                    vm/VM.cpp vm/VM.h
                    vm/definitions.h
                    vm/Operand.h
                    vm/Debugger.cpp vm/Debugger.h)

add_executable(VM ${SOURCE_FILES})