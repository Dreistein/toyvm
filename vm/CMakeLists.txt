cmake_minimum_required(VERSION 3.10)
project(ToyVM)

set(CMAKE_CXX_STANDARD 17)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/out/)
set(CMAKE_EXE_LINKER_FLAGS "-static-libgcc -static-libstdc++ -static")

set(SOURCE_FILES main.cpp Instruction.cpp Instruction.h VM.cpp VM.h definitions.h Operand.h Debugger.cpp Debugger.h)
add_executable(VM ${SOURCE_FILES})