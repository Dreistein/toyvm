//
// Dreistein, Mar. 2018
//

#include <iostream>
#include <sstream>
#include <iterator>
#include "Debugger.h"

using std::string;
using std::vector;
using std::copy;

// Helper Functions
// ============================================================================

/**
 * Converts a string into a vector of strings, delimiter is any kind of whitespace
 * Taken from https://stackoverflow.com/a/237280
 * @param line the line to break apart
 * @return a vector with the separated strings
 */
inline vector<string> tokenize(const string& line) {
    std::istringstream iss(line);
    return { std::istream_iterator<string>{iss},
             std::istream_iterator<string>{}};
}

/**
 * Converts a character to a printable form
 * @param c the character
 * @return The character if it's already printable, a point otherwise
 */
inline char convertPrintable(const char& c) {
    if (c >= 32 and c < 127) // see an ascii table for details
        return c;
    return '.';
}

namespace ToyVM {

    // Private Methods
    // ============================================================================

    /**
     * Prints the help text
     */
    void Debugger::printHelp() const {
        std::cout << "command (shortcut) - description" << std::endl;
        std::cout << "Available commands:" << std::endl;
        std::cout << "\thelp (h)                - shows this information" << std::endl;
        std::cout << "\tbreak (b | bp)          - prints all breakpoints" << std::endl;
        std::cout << "\tbreak (b | bp) <addr>   - toggles a breakpoint at addr" << std::endl;
        std::cout << "\tstep (s)                - executes one instruction and halts again" << std::endl;
        std::cout << "\trun (r)                 - executes until a brekpoint is reached or the machine halts" << std::endl;
        std::cout << "\treg                     - prints the register contents to screen" << std::endl;
        std::cout << "\tmem <addr> <len>        - prints the memory contents of length len to screen" << std::endl;
        std::cout << "\trset <reg> <value>      - sets register reg to value" << std::endl;
        std::cout << "\tmset <addr> <value>     - sets memory address addr to value" << std::endl;
        std::cout << "\texit                    - exits the program" << std::endl;
    }

    /**
     * Prints the register contents of the machine
     */
    void Debugger::printRegister() const {
        std::cout << "== REGISTER CONTENTS ===========================================================\n";
        for(size_t i = 0; i<(REG_SIZE-3); ++i) {
            std::printf("R%02u %04X ", i, machine.REG[i]);
            if(i == (REG_SIZE/2 - 1))
                std::cout << "\n";
        }
        std::printf(" SP %04X  SR %04X  PC %04X\n", machine.REG[SP], machine.REG[SR], machine.REG[PC]);
        // TODO: print decoded status register
    }

    /**
     * Prints the memory contents of the machine
     * @param address_start memory address to start printing
     * @param amount the amount to print
     */
    void Debugger::printMemory(word_t address_start, word_t amount) const {
        std::cout << "== MEMORY CONTENTS =============================================================\n";
        if(address_start + amount >= MEM_SIZE) {
            amount = static_cast<word_t>(REG_SIZE - address_start);
        }
        std::string ascii_val;
        word_t i=0;
        for(; i<amount ; ++i) {

            word_t current_address = address_start + i;
            word_t current_value = machine.MEM[current_address];

            if(i % MEMORY_CELLS_PER_LINE == 0) {
                std::cout << ascii_val << std::endl;
                ascii_val.clear();
                ascii_val.append("; ");
                std::printf("%04X:\t", current_address);
            }

            std::printf("%04X\t", current_value);
            ascii_val += convertPrintable(static_cast<char>(current_value >> 8));
            ascii_val += convertPrintable(static_cast<char>(current_value));
        }

        // fill remaining space with whitespaces
        // to align the ascii values of the last line
        while(i++ % MEMORY_CELLS_PER_LINE != 0) printf("    \t");

        std::cout << ascii_val << std::endl;
    }

    /**
     * Sets the machine memory at addr to value
     * @param addr memory addr
     * @param value value
     */
    void Debugger::setMemory(word_t addr, word_t value) {
        if(addr >= MEM_SIZE)
            return;
        machine.MEM[addr] = value;
    }

    /**
     * Sets the register to value
     *
     * Valid register names are R0-R15, 0-15, SB, SP, SR, PC
     *
     * @param reg register
     * @param value value
     */
    void Debugger::setRegister(string reg, word_t value) {
        if(reg.empty() or reg.size() > 3)
            return;

        word_t r = REG_SIZE;
        if(reg == "SB")
            r = SB;
        else if(reg == "SP")
            r = SP;
        else if(reg == "SR")
            r = SR;
        else if(reg == "PC")
            r = PC;
        else if(reg.find_first_not_of("0123456789") == string::npos)
            r = str_to_i(reg);
        else if((reg[0] == 'R' or reg[0] == 'r') and reg.substr(1).find_first_not_of("0123456789") == string::npos)
            r = str_to_i(reg.substr(1));

        if(r >= REG_SIZE)
            return;
        machine.REG[r] = value;
    }

    /**
     * Executes the next instruction of the VM
     */
    void Debugger::stepOne() {
        if(machine.CPU_HLT) {
            std::cout << "The VM already halted!" << std::endl;
            return;
        }
        machine.step();
    }

    /**
     * Runs the VM until a breakpoint is reached or the VM is halted
     */
    void Debugger::run() {
        machine.step();
        // as long as no breakpoint is found, continue
        auto find = breakpoints.find(machine.REG[PC]);
        while(find == breakpoints.end() and !machine.CPU_HLT) {
            machine.step();
            find = breakpoints.find(machine.REG[PC]);
        }

        if(machine.CPU_HLT) {
            std::cout << "The VM encountered a HLT!" << std::endl;
        } else {
            printf("Breakpoint %04X was reached!\n", *find);
        }
    }

    /**
     *  Toggles a breakpoint
     *  Enables the breakpoint if disabled
     *  Disables the breakpoint if enabled
     * @param addr the memory address of the breakpoint
     */
    void Debugger::toggleBreakpoint(word_t addr) {
        auto found = breakpoints.find(addr);
        if (found == breakpoints.end())
            breakpoints.insert(addr);
        else
            breakpoints.erase(found);
    }

    /**
     * Prints all enabled breakpoints
     */
    void Debugger::listBreakpoints() const {
        std::cout << "== ACTIVE BREAKPOINTS ==========================================================";
        size_t i = 0;
        for (auto &it : breakpoints) {
            if(i % MEMORY_CELLS_PER_LINE == 0) {
                std::cout << "\n\t";
            }
            std::printf("%04X\t", it);
            ++i;
        }
        std::cout << std::endl;
    }

    // Public Methods
    // ============================================================================

    /**
     * Starts the debugger REPL
     */
    void Debugger::start() {
        std::cout << "ToyVM Debugger started...\n";
        std::cout << "Type `help` to get a list of available commands.\ndbg> ";

        while(true) {
            string line;
            std::getline(std::cin, line);
            auto tokens = tokenize(line);

            if(tokens.empty())
                continue;

            auto cmd = tokens[0];

            try { // TODO: rset -> reg <addr> <val>; mset -> memset <addr>  <val> [vals...]
                if (cmd == "h" or cmd == "help") {
                    printHelp();
                } else if (cmd == "reg") {
                    printRegister();
                } else if (cmd == "mem") {
                    if (tokens.size() == 3) {
                        printMemory(str_to_i(tokens[1]), str_to_i(tokens[2]));
                    }
                } else if (cmd == "rset") {
                    if (tokens.size() == 3) {
                        setRegister(tokens[1], str_to_i(tokens[2]));
                    }
                } else if (cmd == "mset") {
                    if (tokens.size() == 3) {
                        setMemory(str_to_i(tokens[1]), str_to_i(tokens[2]));
                    }
                } else if (cmd == "s" or cmd == "step") {
                    stepOne();
                } else if (cmd == "r" or cmd == "run") {
                    run();
                } else if (cmd == "b" or cmd == "bp" or cmd == "break") {
                    if (tokens.size() == 2)
                        toggleBreakpoint(str_to_i(tokens[1]));
                    else
                        listBreakpoints();
                } else if (tokens.at(0) == "exit") {
                    break;
                }
            } catch (std::invalid_argument&) {} // in case the user enters not a number
            std::cout << "dbg> ";
        }
        std::cout << "Exiting the debugger! Goodbye!" << std::endl;
    }

    /**
     * Default constructor
     * @param vm a virtual machine object to debug
     */
    Debugger::Debugger(VM vm) : machine(std::move(vm)) {}
}
