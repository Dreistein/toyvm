//
// Dreistein, Feb. 2018
//

#ifndef VM_OPERAND_H
#define VM_OPERAND_H

#include "definitions.h"

namespace ToyVM {

    // ----------------------------------------------------------------------------
    enum class OperandType : uint8_t {
        VALUE,      // directly represents value
        REGISTER,   // represents value in reg
        ADDRESS,    // represents value at address
    };

    // ----------------------------------------------------------------------------
    // The class does not hold the value, it just tells us where to find the value
    // of the operand
    class Operand {
    private:
        OperandType operand_type;
        word_t operand_value;
    public:
        Operand();

        Operand(OperandType, word_t);

        OperandType type() const;

        word_t val() const;     // unsigned representation
        sword_t sval() const;   // signed representation
    };

    inline Operand::Operand() : operand_type(OperandType::VALUE), operand_value(0) {}

    inline Operand::Operand(OperandType t, word_t v) : operand_type(t), operand_value(v) {}

    inline OperandType Operand::type() const { return operand_type; }

    inline word_t Operand::val() const { return operand_value; }

    inline sword_t Operand::sval() const { return static_cast<sword_t>(operand_value); }

}

#endif //VM_OPERAND_H
