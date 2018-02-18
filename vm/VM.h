//
// Created by Stefan on 17.02.2018.
//

#ifndef HUNGERBYTE_VM_H
#define HUNGERBYTE_VM_H

#include <cstdint>
#include <vector>
#include <functional>
#include "definitions.h"

class Instruction;
class SingleOperandInstruction;
class DualOperandInstruction;
class BranchInstruction;

class Operand;

using input_t = std::function<word_t()>;
using output_t = std::function<void(word_t)>;

class VM {

private:
    word_t REG[REG_SIZE];
    word_t MEM[MEM_SIZE];
    word_t IS_PC;
    bool CPU_HLT = false;
    word_t stub_val;

    std::vector<input_t> input;
    std::vector<output_t> output;

    word_t fetchNextWord();
    Operand decodeOperand(word_t am, word_t n);
    word_t& at(Operand);

    void exec(SingleOperandInstruction);
    void exec(DualOperandInstruction);
    void exec(BranchInstruction);

    status_reg_t getStatusReg() const;
    void setStatusReg(status_reg_t);

public:
    void registerInput(input_t);
    void registerOutput(output_t);
    void run(void);
    void step(void);
    VM(std::vector<word_t> program);
};

#endif //HUNGERBYTE_VM_H
