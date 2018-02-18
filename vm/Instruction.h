//
// Created by Stefan on 17.02.2018.
//

#ifndef HUNGERBYTE_OPERATION_H
#define HUNGERBYTE_OPERATION_H

#include <cstdint>
#include "definitions.h"
#include "Operand.h"

class Instruction {
};

class SingleOperandInstruction : public Instruction {

public:

private:
    word_t instruction;
    single_instr_layout_t decoded;
    Operand dst_operand;

public:
    explicit SingleOperandInstruction(word_t);
    single_instr_layout_t getLayout() const;
    Single_OPC getOpCode() const;
    void setDst(Operand);
    Operand dst() const;
};

class BranchInstruction : public Instruction {

private:
    word_t instruction;
    branch_instr_layout_t decoded;
    Operand dst_address;

public:
    explicit BranchInstruction(word_t instruction);
    branch_instr_layout_t getLayout() const;
    Branch_OPC getOpCode() const;
    void setAddress(word_t address);
    Operand addr() const;
};

class DualOperandInstruction : public Instruction {
private:
    word_t instruction;
    dual_instr_layout_t decoded;
    Operand dst_operand;
    Operand src_operand;
public:
    explicit DualOperandInstruction(word_t instruction);
    dual_instr_layout_t getLayout() const;
    Dual_OPC getOpCode() const;

    void setDst(Operand);
    Operand dst() const;

    void setSrc(Operand);
    Operand src() const;

};

#endif //HUNGERBYTE_OPERATION_H
