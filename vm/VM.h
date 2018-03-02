//
// Dreistein, Feb. 2018
//

#ifndef VM_VM_H
#define VM_VM_H

#include <cstdint>
#include <vector>
#include <functional>
#include "definitions.h"

namespace ToyVM {

    class SingleOperandInstruction;

    class DualOperandInstruction;

    class BranchInstruction;

    class Operand;

    class Debugger;

    using input_t = std::function<word_t()>;
    using output_t = std::function<void(word_t)>;

// ----------------------------------------------------------------------------
    class VM {
    private:
        word_t REG[REG_SIZE];   // Array with register contents
        word_t MEM[MEM_SIZE];   // Array with memory contents
        word_t IS_PC;           // Instruction pointer, at beginning of instruction
        bool CPU_HLT = false;   // Are we done, yet? (Halt flag)
        word_t stub_val;        // Used for value operands

        friend class Debugger;  // Debugger can access the data of the VM now

        // TODO change to array and add standard functions
        std::vector<input_t> input;     // vector with input function
        std::vector<output_t> output;   // vector with output function

        // helper functions
        word_t fetchNextWord();

        Operand decodeOperand(word_t am, word_t n);

        word_t &at(Operand);

        status_reg_t getStatusReg() const;

        void setStatusReg(status_reg_t);

        // execute instructions
        void exec(SingleOperandInstruction);

        void exec(DualOperandInstruction);

        void exec(BranchInstruction);

    public:
        explicit VM(std::vector<word_t> program);

        void run();

        void step();

        void registerInput(input_t);

        void registerOutput(output_t);
    };
}

#endif //VM_VM_H
