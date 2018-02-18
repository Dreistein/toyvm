//
// Created by Stefan on 17.02.2018.
//

#include <stdexcept>
#include <cstring>
#include "Instruction.h"
#include "definitions.h"


SingleOperandInstruction::SingleOperandInstruction(word_t instruction) : instruction(instruction), dst_operand(Operand(OperandType::VALUE, 0)){
    // packing into BitField, not TypeSafe but if it doesn't fit, something is wrong with our logic.
    //this->decoded = *(op_layout *)this->instruction;
    memcpy(&this->decoded, &this->instruction, sizeof(single_instr_layout_t));
    if(this->decoded.id != 0) {
        throw std::logic_error("The type of the instruction doesn't match the expected instruction");
    }
}

single_instr_layout_t SingleOperandInstruction::getLayout() const {
    return this->decoded;
}

Single_OPC SingleOperandInstruction::getOpCode() const {
    return static_cast<Single_OPC>(this->decoded.opc);
}

void SingleOperandInstruction::setDst(Operand dst) {
    this->dst_operand = dst;
}

Operand SingleOperandInstruction::dst() const {
    return dst_operand;
}


BranchInstruction::BranchInstruction(word_t instruction) : instruction(instruction), dst_address(Operand(OperandType::VALUE, 0)) {
    // packing into BitField, not TypeSafe but if it doesn't fit, something is wrong with our logic.
    //this->decoded = *(op_layout *)this->instruction;
    memcpy(&this->decoded, &this->instruction, sizeof(branch_instr_layout_t));
    if(this->decoded.id != 1) {
        throw std::logic_error("The type of the instruction doesn't match the expected instruction");
    }
    this->dst_address = Operand(OperandType::VALUE, this->decoded.offset); //check if offset correct
}
branch_instr_layout_t BranchInstruction::getLayout() const {
    return this->decoded;
}

Branch_OPC BranchInstruction::getOpCode() const {
    return static_cast<Branch_OPC>(this->decoded.condition);
}

void BranchInstruction::setAddress(word_t address) {
    dst_address = Operand(OperandType::VALUE, address);
}

Operand BranchInstruction::addr() const {
    return dst_address;
}


DualOperandInstruction::DualOperandInstruction(word_t instruction) : instruction(instruction),
dst_operand(Operand(OperandType::VALUE, 0)), src_operand(Operand(OperandType::VALUE, 0)) {
    // packing into BitField, not TypeSafe but if it doesn't fit, something is wrong with our logic.
    //this->decoded = *(op_layout *)this->instruction;
    memcpy(&this->decoded, &this->instruction, sizeof(dual_instr_layout_t));
    if(this->decoded.id != 1) {
        throw std::logic_error("The type of the instruction doesn't match the expected instruction");
    }
}

dual_instr_layout_t DualOperandInstruction::getLayout() const {
    return this->decoded;
}

Dual_OPC DualOperandInstruction::getOpCode() const {
    return static_cast<Dual_OPC>(this->decoded.opc);
}

void DualOperandInstruction::setDst(Operand op) {
    this->dst_operand = op;
}
Operand DualOperandInstruction::dst() const {
    return this->dst_operand;
}

void DualOperandInstruction::setSrc(Operand op) {
    this->src_operand = op;
}

Operand DualOperandInstruction::src() const {
    return this->src_operand;
}