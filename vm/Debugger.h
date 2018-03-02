//
// Dreistein, Mar. 2018
//

#ifndef TOYVM_DEBUGGER_H
#define TOYVM_DEBUGGER_H

#include "definitions.h"
#include "VM.h"

namespace ToyVM {

    class Debugger {
        VM machine;

    private:
        void printHelp() const;
        void printRegister() const;
        void printMemory(word_t, word_t) const;
        void setMemory(word_t, word_t);
        void setRegister(std::string, word_t);

    public:
        explicit Debugger(VM);
        void start();
    };
}

#endif //TOYVM_DEBUGGER_H
