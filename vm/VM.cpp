//
// Created by Stefan on 17.02.2018.
//

#include <iostream>
#include <cstring>

#include "VM.h"
#include "Instruction.h"
#include "definitions.h"
#include "Operand.h"

status_reg_t VM::getStatusReg() const {
    status_reg_t sr {0};
    std::memcpy(&sr, this->REG+SR, sizeof(status_reg_t));
    return sr;
}

void VM::setStatusReg(status_reg_t sr) {
    std::memcpy(this->REG+SR, &sr, sizeof(status_reg_t));
}

word_t VM::fetchNextWord() {
    return this->MEM[this->REG[PC]++];
}

word_t& VM::at(const Operand op) {
    switch (op.type()) {
        case OperandType::ADDRESS : return this->MEM[op.val()];
        case OperandType::REGISTER : return this->REG[op.val()];
        case OperandType::VALUE : return stub_val = op.val();
    }
}

Operand VM::decodeOperand(word_t am, word_t n) { //TODO: make am and n there own type
    if(n == SR) {
        switch (am) {
            case 0b00:  return Operand(OperandType::REGISTER, SR);
            case 0b01:  return Operand(OperandType::VALUE, IS_PC + fetchNextWord());
            case 0b10:  return Operand(OperandType::VALUE, fetchNextWord());
            case 0b11:  return Operand(OperandType::ADDRESS, fetchNextWord());
            default: ;
        }
    }
    switch (am) {
        case 0b00: return Operand(OperandType::REGISTER, n);
        case 0b01: return Operand(OperandType::VALUE, this->REG[n] + fetchNextWord());
        case 0b10: return Operand(OperandType::ADDRESS, this->REG[n]);
        case 0b11: return Operand(OperandType::ADDRESS, this->REG[n] + fetchNextWord());
        default: ;
    }
    return Operand(OperandType::VALUE, 0);
}

void VM::exec(BranchInstruction instruction) {
    bool condition = false;
    auto sr = getStatusReg();
    switch (instruction.getOpCode()) {
        case Branch_OPC::JMP    : condition = true;                 break;
        case Branch_OPC::JN     : condition = sr.n == 1;            break;
        case Branch_OPC::JGE    : condition = (sr.n ^ sr.v) == 0;   break;
        case Branch_OPC::JL     : condition = (sr.n ^ sr.v) == 1;   break;
        case Branch_OPC::JNE    : condition = sr.z == 0;            break;
        case Branch_OPC::JEQ    : condition = sr.z == 1;            break;
        case Branch_OPC::JNC    : condition = sr.c == 0;            break;
        case Branch_OPC::JC     : condition = sr.c == 1;            break;
        default: condition = false;
    }
    if(condition) {
        this->REG[PC] = at(instruction.addr());
    }
    this->REG[SR] = 0;
}

void VM::exec(SingleOperandInstruction instruction) {
    auto cached = static_cast<sword_t>(at(instruction.dst()));

    auto opc = instruction.getOpCode();
    auto decoded = instruction.getLayout();
    auto dst = instruction.dst();

    switch (opc) {
        case Single_OPC::HLT    : CPU_HLT = true; break;
        case Single_OPC::PUSH   : MEM[REG[SP]--] = at(dst); break;
        case Single_OPC::POP    : at(dst) = MEM[REG[SP]++]; break;
        case Single_OPC::CALL   : MEM[REG[SP]--] = REG[PC]; REG[PC] = at(dst); break;
        case Single_OPC::RET    : REG[PC] = MEM[REG[SP]++]; break;
        case Single_OPC::INC    : at(dst)++; break;
        case Single_OPC::DEC    : at(dst)--; break;
        case Single_OPC::INV    : at(dst) = ~at(dst); break;
        case Single_OPC::SHR    : at(dst) = at(dst) >> 1; break;
        case Single_OPC::SHL    : at(dst) = at(dst) << 1; break;
        case Single_OPC::OUT    : std::cout << at(dst);
        case Single_OPC::IN     : std::cin  >> at(dst);
        default: ;
    }

    auto after = static_cast<sword_t>(at(dst));
    status_reg_t sr {0};

    switch (instruction.getOpCode()) {
        case Single_OPC::INC:
            if(cached >= 0 and after < 0)
                sr.v = 1;
        case Single_OPC::DEC:
            if(cached < 0 and after >= 0)
                sr.v = 1;
        case Single_OPC::INV:
        case Single_OPC::SHR:
        case Single_OPC::SHL:
        case Single_OPC::TST:
            if(after < 0)
                sr.n = 1;
            if(after == 0)
                sr.z = 1;
            setStatusReg(sr);
            break;
        default:
            this->REG[SR] = 0;
    }
}

void VM::exec(DualOperandInstruction instruction) {
    int64_t result;
    switch (instruction.getOpCode()) {
        case Dual_OPC::MOV  :   result = at(instruction.src()); break;
        case Dual_OPC::ADD  :   result = at(instruction.dst()) + at(instruction.src()); break;
        case Dual_OPC::SUB  :   result = at(instruction.dst()) - at(instruction.src()); break;
        case Dual_OPC::MUL  :   result = at(instruction.dst()) * at(instruction.src()); break;
        case Dual_OPC::AND  :   result = at(instruction.dst()) & at(instruction.src()); break;
        case Dual_OPC::OR   :   result = at(instruction.dst()) | at(instruction.src()); break;
        case Dual_OPC::XOR  :   result = at(instruction.dst()) ^ at(instruction.src()); break;
        case Dual_OPC::SHR  :   result = at(instruction.dst()) >> at(instruction.src()); break;
    }

    auto upper_result = static_cast<uint64_t>(result >> sizeof(word_t)*8);

    status_reg_t sr {0};

    if(upper_result == 1)
        sr.c = 1;
    if(upper_result > 1)
        sr.v = 1;
    if(result == 0)
        sr.z = 1;
    if(result < 0)
        sr.n = 1;

    if(instruction.getOpCode() == Dual_OPC::MUL)
        sr.rem_bits = static_cast<word_t>(upper_result);

    setStatusReg(sr);
    at(instruction.dst()) = static_cast<word_t>(result);
}

void VM::step() {

    if (CPU_HLT) return;

    this->IS_PC = this->REG[PC]; // save pc at instruction start
    word_t instruction_word = fetchNextWord();

    // Dual Operand instruction
    if(IS_DUAL_IS(instruction_word)) {
        auto instr = DualOperandInstruction(instruction_word);
        auto layout = instr.getLayout();
        instr.setDst(decodeOperand(layout.ad, layout.dst));
        instr.setSrc(decodeOperand(layout.as, layout.src));
        exec(instr);
    }
    // Branch Instruction
    if(IS_BRANCH_IS(instruction_word)) {
        auto instr = BranchInstruction(instruction_word);
        auto layout = instr.getLayout();
        if(layout.v)
            instr.setAddress(fetchNextWord());
        exec(instr);
    }

    if(IS_SINGLE_IS(instruction_word)) {
        // Single Operand Instruction
        auto instr = SingleOperandInstruction(instruction_word);
        auto layout = instr.getLayout();
        instr.setDst(decodeOperand(layout.as, layout.src));
        exec(instr);
    }
    if(REG[PC] == 0xFFFF)
        CPU_HLT = true;
}

void VM::run() {
    while(!CPU_HLT) {
        step();
    }
}

VM::VM(std::vector<word_t> program) {
    REG[PC] = PC_START;
    REG[SP] = SP_START + 1;
    MEM[SP_START] = 0xFFFF;

    std::memcpy(MEM+PC_START, program.data(), sizeof(word_t)*program.size());
}
