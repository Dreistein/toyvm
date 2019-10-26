//
// Dreistein, Feb. 2018
//

#include <iostream>
#include <fstream>

#include "definitions.h"
#include "VM.h"
#include "util.h"

#ifdef VM_DEBUG
#include "Debugger.h"
#endif

using ToyVM::word_t;

int main(int argc, const char *argv[]) {

    if (argc != 2) {
        std::cout << "Usage: vm <program>" << std::endl;
        return 0;
    }

    std::cout << "Started VM\nLoading program from " << argv[1] << std::endl;
    const std::string filename(argv[1]);
    ToyVM::VM vm(readFile(filename));

    auto stdinBuf = readFile("stdin.txt");
    // register default lambdas for input / output operation (port 0)
    vm.registerInput( [&stdinBuf]() {
        word_t word = 0;
        if(!stdinBuf.empty()) {
            word = stdinBuf.back();
            stdinBuf.pop_back();
        }
        return word;
    });
    std::fstream stdoutFile;
    stdoutFile.open("stdout.txt", std::fstream::out);
    vm.registerOutput( [&stdoutFile](word_t out) {
        stdoutFile << out << ' ';
    });
    // register character io (port 1)
    vm.registerInput( [&stdinBuf]() {
        word_t word = 0;
        if(!stdinBuf.empty()) {
            word = stdinBuf.back();
            stdinBuf.pop_back();
        }
        return word;
    });
    vm.registerOutput( [&stdoutFile](word_t out) {
        stdoutFile << static_cast<char>(out);
    });

#ifdef VM_DEBUG
    ToyVM::Debugger debugger(vm);
    debugger.start();
#else
    vm.run();
#endif

    // close output stream
    stdoutFile.close();
    return 0;
}