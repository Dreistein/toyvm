//
// Created by Stefan on 17.02.2018.
//

#ifndef HUNGERBYTE_VM_H
#define HUNGERBYTE_VM_H

#include <cstdint>
#include <vector>
#include "definitions.h"

class Instruction;
class SingleOperandInstruction;
class DualOperandInstruction;
class BranchInstruction;

class Operand;

class VM {

private:
    word_t REG[REG_SIZE];
    word_t MEM[MEM_SIZE];
    word_t IS_PC;
    bool CPU_HLT = false;
    word_t stub_val;

    word_t fetchNextWord();
    Operand decodeOperand(word_t am, word_t n);
    word_t& at(Operand);

    void exec(SingleOperandInstruction);
    void exec(DualOperandInstruction);
    void exec(BranchInstruction);

    status_reg_t getStatusReg() const;
    void setStatusReg(status_reg_t);

public:
    void run(void);
    void step(void);
    VM(std::vector<word_t> program);
};

#endif //HUNGERBYTE_VM_H
