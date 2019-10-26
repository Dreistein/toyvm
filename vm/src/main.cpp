//
// Dreistein, Feb. 2018
//

#include <iostream>
#include <fstream>

#include "definitions.h"
#include "VM.h"
#include "util.h"
#include "OutputWriter.h"
#include "InputReader.h"

#ifdef VM_DEBUG
#include "Debugger.h"
#endif

using ToyVM::word_t;

constexpr auto opt_use_stdin  = "--stdin";
constexpr auto opt_use_stdout = "--stdout";

void print_usage() {
    std::cout << "Usage: vm <program> [" << opt_use_stdout << "] [" << opt_use_stdin << "]" << std::endl;
    exit(1);
}

void parse_arguments(int argc, const char *argv[], std::string& filename, ToyVM::InputReader*& in, ToyVM::OutputWriter*& out) {
    bool found_filename = false;
    bool use_stdin = false;
    bool use_stdout = false;

    for(size_t i = 1; i < argc; ++i) {
        std::string argument(argv[i]);
        if(argument.compare(opt_use_stdout) == 0) {
            use_stdout = true;
        }
        else if(argument.compare(opt_use_stdin) == 0) {
            use_stdin = true;
        }
        else if(!found_filename) {
            found_filename = true;
            filename = argument;
        } else {
            std::cout << "Unknown argument '" << argument << "'" << std::endl;
            print_usage();
        }
    }

    if(!found_filename) print_usage();

    if(use_stdin) {
        std::cout << "Using stdin as input" << std::endl;
        in = new ToyVM::InputReader();
    } else {
        std::cout << "Using \"stdin.txt\" as input" << std::endl;
        in = new ToyVM::InputReader("stdout.txt");
    }

    if(use_stdout) {
        std::cout << "Using stdout as output" << std::endl;
        out = new ToyVM::OutputWriter();
    } else {
        std::cout << "Using \"stdout.txt\" as output" << std::endl;
        out = new ToyVM::OutputWriter("stdout.txt");
    }
}

int main(int argc, const char *argv[]) {

    if (argc < 2) {
        print_usage();
    }

    std::string filename;
    ToyVM::OutputWriter* outFile;
    ToyVM::InputReader* inFile;

    parse_arguments(argc, argv, filename, inFile, outFile);

    std::cout << "Starting VM\nLoading program from " << filename << std::endl;
    ToyVM::VM vm(readFile(filename));

    // register default lambdas for input / output operation (port 0)
    vm.registerInput( [&inFile]() {
        word_t word = 0;
        *inFile >> word;
        return word;
    });

    vm.registerOutput( [&outFile](word_t out) {
        *outFile << out << ' ';
    });
    // register character io (port 1)
    vm.registerInput( [&inFile]() {
        word_t word = 0;
        *inFile >> word;
        return word;
    });
    vm.registerOutput( [&outFile](word_t out) {
        *outFile << static_cast<char>(out);
    });

#ifdef VM_DEBUG
    ToyVM::Debugger debugger(vm);
    debugger.start();
#else
    vm.run();
#endif

    // close output stream
    delete outFile;
    delete inFile;
    return 0;
}