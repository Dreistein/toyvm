//
// Dreistein, Feb. 2018
//

#include <iostream>
#include <cstring>

#include "VM.h"
#include "Instruction.h"
#include "definitions.h"
#include "Operand.h"

namespace ToyVM {

    // Public Methods
    // ============================================================================

    /**
     * Instantiates the VM with a program to execute
     * @param program
     */
    VM::VM(std::vector<word_t> program) {
        REG[PC] = PC_START;
        REG[SP] = SP_START;
        MEM[SP_START] = 0xFFFF;

        std::memcpy(MEM + PC_START, program.data(), sizeof(word_t) * program.size());
    }

    /**
     * Runs the program until a CPU HLT is encountered
     */
    void VM::run() {
        while (!CPU_HLT) {
            step();
        }
    }

    /**
     * Fetches and executes a single instruction
     */
    void VM::step() {

        if (CPU_HLT) return;

        this->IS_PC = this->REG[PC]; // save pc at instruction start
        word_t instruction_word = fetchNextWord();

        // Dual Operand instruction
        if (IS_DUAL_IS(instruction_word)) {
            // decode
            auto instr = DualOperandInstruction(instruction_word);
            auto layout = instr.getLayout();
            // decode operands
            instr.setDst(decodeOperand(layout.ad, layout.dst));
            instr.setSrc(decodeOperand(layout.as, layout.src));
            // execute
            exec(instr);
        }
        // Branch Instruction
        if (IS_BRANCH_IS(instruction_word)) {
            // decode
            auto instr = BranchInstruction(instruction_word);
            auto layout = instr.getDecoded();
            // decode / fetch branch address
            if (layout.v)
                instr.setAddress(fetchNextWord());
            // execute
            exec(instr);
        }

        // Single Operand Instruction
        if (IS_SINGLE_IS(instruction_word)) {
            // decode
            auto instr = SingleOperandInstruction(instruction_word);
            auto layout = instr.getDecoded();
            // decode operand
            instr.setDst(decodeOperand(layout.as, layout.src));
            // execute
            exec(instr);
        }

        // if the PC reaches the highest possible address, the cpu halts
        if (REG[PC] == 0xFFFF)
            CPU_HLT = true;
    }

    /**
     * Registers function for input io
     * @param i the function used for input
     */
    void VM::registerInput(input_t i) {
        input.push_back(i);
    }

    /**
     * Registers function for output io
     * @param o the function used for input
     */
    void VM::registerOutput(output_t o) {
        output.push_back(o);
    }

    // Instruction execution
    // ============================================================================

    /**
     * Executes a Single Operand Instruction
     * @param instruction
     */
    void VM::exec(SingleOperandInstruction instruction) {
        auto cached = static_cast<sword_t>(at(instruction.dst()));

        auto opc = instruction.getOpCode();
        auto decoded = instruction.getDecoded();
        auto dst = instruction.dst();

        switch (opc) {
            case Single_OPC::HLT    : CPU_HLT = true; break;
            case Single_OPC::PUSH   : MEM[--REG[SP]] = at(dst); break;
            case Single_OPC::POP    : at(dst) = MEM[REG[SP]++]; break;
            case Single_OPC::CALL   : MEM[--REG[SP]] = REG[PC]; REG[PC] = at(dst); break;
            case Single_OPC::RET    : REG[PC] = MEM[REG[SP]++]; break;
            case Single_OPC::INC    : at(dst)++; break;
            case Single_OPC::DEC    : at(dst)--; break;
            case Single_OPC::INV    : at(dst) = ~at(dst); break;
            case Single_OPC::SHR    : at(dst) = at(dst) >> 1; break;
            case Single_OPC::SHL    : at(dst) = at(dst) << 1; break;
            case Single_OPC::OUT    : output.at(decoded.port)(at(dst)); break;
            case Single_OPC::IN     : at(dst) = input.at(decoded.port)(); break;
            default:;
        }

        // update status register
        auto after = static_cast<sword_t>(at(dst));
        status_reg_t sr{0};

        switch (instruction.getOpCode()) {
            case Single_OPC::INC:
                if (cached >= 0 and after < 0)
                    sr.v = 1;
            case Single_OPC::DEC:
                if (cached < 0 and after >= 0)
                    sr.v = 1;
            case Single_OPC::INV:
            case Single_OPC::SHR:
            case Single_OPC::SHL:
            case Single_OPC::TST:
                if (after < 0)
                    sr.n = 1;
                if (after == 0)
                    sr.z = 1;
                setStatusReg(sr);
                break;
            default:;
        }
    }

    /**
     * Executes a Branch Instruction
     * @param instruction
     */
    void VM::exec(BranchInstruction instruction) {
        bool condition = false;
        auto sr = getStatusReg();
        switch (instruction.getOpCode()) {
            case Branch_OPC::JMP    : condition = true; break;
            case Branch_OPC::JN     : condition = sr.n == 1; break;
            case Branch_OPC::JGE    : condition = (sr.n ^ sr.v) == 0; break;
            case Branch_OPC::JL     : condition = (sr.n ^ sr.v) == 1; break;
            case Branch_OPC::JNE    : condition = sr.z == 0; break;
            case Branch_OPC::JEQ    : condition = sr.z == 1; break;
            case Branch_OPC::JNC    : condition = sr.c == 0; break;
            case Branch_OPC::JC     : condition = sr.c == 1; break;
        }
        if (condition) {
            this->REG[PC] = IS_PC + instruction.addr().sval(); //FIXME: what about extra words that are absolute?
        }
    }

    /**
     * Executes a Dual Operand Instruction
     * @param instruction
     */
    void VM::exec(DualOperandInstruction instruction) {
        int64_t result;
        auto opc = instruction.getOpCode();
        sword_t dst = at(instruction.dst());
        sword_t src = at(instruction.src());

        switch (opc) {
            case Dual_OPC::MOV  : result = src; break;
            case Dual_OPC::ADD  : result = dst + src; break;
            case Dual_OPC::SUB  : result = dst - src; break;
            case Dual_OPC::MUL  : result = dst * src; break;
            case Dual_OPC::AND  : result = dst & src; break;
            case Dual_OPC::OR   : result = dst | src; break;
            case Dual_OPC::XOR  : result = dst ^ src; break;
            case Dual_OPC::SHR  : result = dst >> src; break;
        }

        auto upper_result = static_cast<sword_t>(result >> sizeof(word_t) * 8);
        auto lower_result = static_cast<sword_t>(result);

        status_reg_t sr{0};

        // checks if the operands have the same/different signs
        // used for overflow bit. it's only a overflow if both operands have the
        // same sign or a different sign when subtracting
        bool sign_indicator = ( (src > 0)  == (dst > 0) ) ^ Dual_OPC::SUB == opc;

        sr.c = static_cast<word_t>( upper_result == 1 );
        sr.v = static_cast<word_t>( (upper_result > 1) & sign_indicator);
        sr.z = static_cast<word_t>( lower_result == 0 );
        sr.n = static_cast<word_t>( lower_result  < 0 );


        if (instruction.getOpCode() == Dual_OPC::MUL)
            sr.rem_bits = static_cast<word_t>(upper_result);

        setStatusReg(sr);
        at(instruction.dst()) = static_cast<word_t>(result);
    }

    // Helper methods
    // ============================================================================


    /**
     * Decodes the current status register
     * @return The current status register in a convenient bit-field
     */
    status_reg_t VM::getStatusReg() const {
        status_reg_t sr{0};
        std::memcpy(&sr, this->REG + SR, sizeof(status_reg_t));
        return sr;
    }

    /**
     * Updates the current status register to the given one
     * @param sr the new status register contents
     */
    void VM::setStatusReg(status_reg_t sr) {
        std::memcpy(this->REG + SR, &sr, sizeof(status_reg_t));
    }

    /**
     * Fetches the next (instruction) word from memory
     * and increments the program counter
     * @return the next instruction word
     */
    word_t VM::fetchNextWord() {
        return this->MEM[this->REG[PC]++];
    }

    /**
     * Decodes an operand and returns the reference to the
     * word the operand is pointing to
     * @param op the operand to decode
     * @return a reference to the operand word
     */
    word_t &VM::at(const Operand op) {
        switch (op.type()) {
            case OperandType::ADDRESS :
                return this->MEM[op.val()];
            case OperandType::REGISTER :
                return this->REG[op.val()];
            case OperandType::VALUE :
                return stub_val = op.val();
        }
    }

    /**
     * Decodes raw operand information into a Operand class
     * @param am The address-mode used
     * @param n the register selected
     * @return a Operand object with the decoded information
     */
    Operand VM::decodeOperand(word_t am, word_t n) { //TODO: make am and n there own type
        if (n == SR) {
            switch (am) {
                case 0b00:
                    return Operand(OperandType::REGISTER, SR);
                case 0b01:
                    return Operand(OperandType::VALUE, fetchNextWord());
                case 0b10:
                    return Operand(OperandType::VALUE, IS_PC + fetchNextWord());
                case 0b11:
                    return Operand(OperandType::ADDRESS, fetchNextWord());
                default:;
            }
        }
        switch (am) {
            case 0b00:
                return Operand(OperandType::REGISTER, n);
            case 0b01:
                return Operand(OperandType::VALUE, this->REG[n] + fetchNextWord());
            case 0b10:
                return Operand(OperandType::ADDRESS, this->REG[n]);
            case 0b11:
                return Operand(OperandType::ADDRESS, this->REG[n] + fetchNextWord());
            default:;
        }
        return Operand(OperandType::VALUE, 0);
    }
}