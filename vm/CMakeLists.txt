cmake_minimum_required(VERSION 3.10)
project(HungerByte)

set(CMAKE_CXX_STANDARD 17)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "C:/Users/Stefan/Documents/stuff/hunger-byte-vm/vm/Release/")

set(SOURCE_FILES main.cpp Instruction.cpp Instruction.h VM.cpp VM.h definitions.h Operand.h)
add_executable(HungerByte ${SOURCE_FILES})