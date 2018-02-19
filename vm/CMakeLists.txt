cmake_minimum_required(VERSION 3.10)
project(HungerByte)

set(CMAKE_CXX_STANDARD 17)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ../Release)
set(CMAKE_EXE_LINKER_FLAGS "-static-libgcc -static-libstdc++ -static")

set(SOURCE_FILES main.cpp Instruction.cpp Instruction.h VM.cpp VM.h definitions.h Operand.h)
add_executable(HungerByte ${SOURCE_FILES})