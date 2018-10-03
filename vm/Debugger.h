//
// Dreistein, Mar. 2018
//

#ifndef TOYVM_DEBUGGER_H
#define TOYVM_DEBUGGER_H

#include <set>
#include "definitions.h"
#include "VM.h"

// used in printMemory
// defines how many memory cells are printed per line
#define MEMORY_CELLS_PER_LINE 6

namespace ToyVM {

    class Debugger {
        VM machine;                     // the VM which is being debugged
        std::set<word_t> breakpoints;   // all active breakpoints

    private:
        void printHelp() const;
        void printRegister() const;
        void printMemory(word_t, word_t) const;
        void setMemory(std::vector<std::string>);
        void setRegister(std::string, word_t);
        void stepOne();
        void run();
        void toggleBreakpoint(word_t addr);
        void listBreakpoints() const;

    public:
        explicit Debugger(VM);
        void start();
    };
}

#endif //TOYVM_DEBUGGER_H
