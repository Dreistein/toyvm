#include <iostream>

#include "VM.h"

int main() {
    std::vector<word_t> prog;
    prog.push_back(0b0001010000000101);
    prog.push_back(0b0001010000000101);
    prog.push_back(0b0010100000000101);
    prog.push_back(0b0001110000000101);
    VM vm(prog);
    //vm.step();
    //vm.step();
    //vm.step();
    vm.run();
    return 0;
}