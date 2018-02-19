//
// Dreistein, Feb. 2018
//
// This file contains the class definitions representing the instructions
//

#ifndef VM_INSTRUCTION_H
#define VM_INSTRUCTION_H

#include <cstdint>
#include "definitions.h"
#include "Operand.h"

namespace ToyVM {

    // ----------------------------------------------------------------------------
    class Instruction {
    protected:
        // contains the raw instruction word
        // this is for debug purposes at the moment
        word_t instruction;
    public:
        Instruction(word_t);
    };


    // ----------------------------------------------------------------------------
    class SingleOperandInstruction : public Instruction {
    private:
        // the instruction word decoded into a bitfield
        single_instr_layout_t decoded;
        // the destination operand
        Operand dst_operand;

    public:
        explicit SingleOperandInstruction(word_t);

        single_instr_layout_t getDecoded() const;
        Single_OPC getOpCode() const;

        void setDst(Operand);
        Operand dst() const;
    };

    // ----------------------------------------------------------------------------
    class BranchInstruction : public Instruction {
    private:
        // the instruction word decoded into a bitfield
        branch_instr_layout_t decoded;
        // the destination address of the branch
        Operand dst_address;

    public:
        explicit BranchInstruction(word_t instruction);

        branch_instr_layout_t getDecoded() const;
        Branch_OPC getOpCode() const;

        void setAddress(word_t address);
        Operand addr() const;
    };

    // ----------------------------------------------------------------------------
    class DualOperandInstruction : public Instruction {
    private:
        // the instruction word decoded into a bitfield
        dual_instr_layout_t decoded;
        // the destination operand
        Operand dst_operand;
        // the source operand
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

}

#endif //VM_INSTRUCTION_H
