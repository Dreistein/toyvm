//
// Dreistein, Feb. 2018
//

#include <stdexcept>
#include <cstring>
#include "Instruction.h"
#include "definitions.h"

namespace ToyVM {

    Instruction::Instruction(word_t instruction) : instruction(instruction) {}

    /**
     * Instantiates a Single Operand Instruction and decodes the instruction word
     * @param instruction the raw instruction word
     * @throw logic_error if instruction is not a valid single operand instruction
     */
    SingleOperandInstruction::SingleOperandInstruction(word_t instruction) :
            Instruction(instruction) {

        // packing into BitField, not TypeSafe but if it doesn't fit,
        // something is wrong with our logic.
        memcpy(&this->decoded, &this->instruction, sizeof(single_instr_layout_t));

        // if the instruction word doesn't match the layout, an exception is thrown,
        // indicating some internal program error
        if (this->decoded.id != 0b00) {
            throw std::logic_error("The type of the instruction doesn't match the expected instruction");
        }
    }

    /**
     * Instantiates a Branch Instruction and decodes it
     * @param instruction the raw instruction word
     * @throw logic_error if instruction is not a valid branch instruction
     */
    BranchInstruction::BranchInstruction(word_t instruction) :
            Instruction(instruction) {

        // packing into BitField, not TypeSafe but if it doesn't fit,
        // something is wrong with our logic.
        memcpy(&this->decoded, &this->instruction, sizeof(branch_instr_layout_t));

        // if the instruction word doesn't match the layout, an exception is thrown,
        // indicating some internal program error
        if (this->decoded.id != 0b01) {
            throw std::logic_error("The type of the instruction doesn't match the expected instruction");
        }

        // calculate offset with sign extension
        // needs to be a signed type. the right shift only fills with 1's on signed types
        sword_t offset = (static_cast<sword_t>(this->decoded.offset) << 6);
        offset >>= 6;
        this->dst_address = Operand(OperandType::VALUE, static_cast<word_t>(offset));
    }


    /**
     * Instantiates a Dual Operand Instruction and decodes it
     * @param instruction
     * @throw logic_error if instruction is not a valid dual operand instruction
     */
    DualOperandInstruction::DualOperandInstruction(word_t instruction) :
            Instruction(instruction) {

        // packing into BitField, not TypeSafe but if it doesn't fit, something is wrong with our logic.
        //this->decoded = *(op_layout *)this->instruction;
        memcpy(&this->decoded, &this->instruction, sizeof(dual_instr_layout_t));

        // if the instruction word doesn't match the layout, an exception is thrown,
        // indicating some internal program error
        if (this->decoded.id != 1) {
            throw std::logic_error("The type of the instruction doesn't match the expected instruction");
        }
    }

    // -------------------------------- Getter / Setter ---------------------------

    // -------------------------------- Single OP -----------------

    single_instr_layout_t SingleOperandInstruction::getDecoded() const {
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

    // -------------------------------- Branch OP -----------------

    branch_instr_layout_t BranchInstruction::getDecoded() const {
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

    // -------------------------------- Dual OP -------------------

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

}