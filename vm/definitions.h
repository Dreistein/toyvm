//
// Dreistein, Feb. 2018
//
// This file contains some useful definitions
// of memory layout, op codes and constants used
// by the VM
//

#ifndef VM_DEFINITIONS_H
#define VM_DEFINITIONS_H

#include <cstdint>

namespace ToyVM {

// Initialization Values
#define PC_START 0x0100
#define SP_START 0xEEEE
#define MEM_SIZE 0x10000
#define REG_SIZE 16

// Common names for Registers
#define SB 0xC
#define SP 0xD
#define SR 0xE
#define PC 0xF

    // denotes a single word
    typedef uint16_t word_t;
    typedef int16_t sword_t;

    // layout of status reg
    typedef struct {
        word_t rem_bits :12;
        word_t c :1; // carry bit
        word_t z :1; // zero bit
        word_t n :1; // negative bit
        word_t v :1; // overflow bit
    } status_reg_t;

// Dual Operand Instruction
// ===========================================================================

// macro checks if instruction word represents a dual operand instruction
#define IS_DUAL_IS(IS) ((IS) & 0x8000)

    // Available Op codes
    enum class Dual_OPC : uint8_t {
        MOV, ADD, SUB, MUL, AND, OR, XOR, SHR
    };

    // memory layout of dual operand instructions
    typedef struct {
        word_t src :4;
        word_t dst :4;
        word_t as  :2;
        word_t ad  :2;
        word_t opc :3;
        word_t id  :1;
    } dual_instr_layout_t;


// Branch Instruction
// ===========================================================================

// macro checks if instruction word represents a branching instruction
#define IS_BRANCH_IS(IS) (((IS) & 0xC000) == 0x4000)

    // Available op codes
    enum class Branch_OPC : uint8_t {
        JMP, JN, JGE, JL, JNE, JEQ, JNC, JC
    };

    // memory layout of branching instructions
    typedef struct {
        word_t offset      :10;
        word_t v           :1;
        word_t condition   :3;
        word_t id          :2;
    } branch_instr_layout_t;



// Single Operand Instruction
// ===========================================================================

// macro checks if instruction word represents a single operand instruction
#define IS_SINGLE_IS(IS) (((IS) & 0xC000) == 0x0000)

    // available op codes
    enum class Single_OPC : uint8_t {
        HLT, PUSH, POP, CALL, RET, //Stack operations
        INC, DEC,
        INV, TST, SHR, SHL,
        OUT, IN,
    };

    // memory layout of single operand instructions
    typedef struct {
        word_t src  :4;
        word_t as   :2;
        word_t port :4;
        word_t opc  :4;
        word_t id   :2;
    } single_instr_layout_t;

} // end namespace ToyVM

// converts a number literal to a word
inline ToyVM::word_t str_to_i(const std::string& line) {
    if (line.substr(0, 2) == "0b")
        return static_cast<ToyVM::word_t>(std::stoi(line.substr(2), nullptr, 2));
    else
        return static_cast<ToyVM::word_t>(std::stoi(line, nullptr, 0));
}

#endif //VM_DEFINITIONS_H
