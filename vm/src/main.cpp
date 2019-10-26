//
// Dreistein, Feb. 2018
//

#include <iostream>
#include <fstream>

#include "definitions.h"
#include "VM.h"
#include "util.h"
#include "OutputWriter.h"

#ifdef VM_DEBUG
#include "Debugger.h"
#endif

using ToyVM::word_t;

constexpr auto opt_use_stdin  = "--stdout";
constexpr auto opt_use_stdout = "--stdout";

void print_usage() {
#ifdef VM_DEBUG
    std::cout << "Usage: vm <program> [" << opt_use_stdout << "]" << std::endl; 
#else
    std::cout << "Usage: vm <program> [" << opt_use_stdout << "] [" << opt_use_stdin << "]" << std::endl;
#endif
    exit(1);
}

void parse_arguments(int argc, const char *argv[], std::string& filename, ToyVM::OutputWriter*& out) {
    bool found_filename = false;
    bool use_stdin = false;
    bool use_stdout = false;

    for(size_t i = 1; i < argc; ++i) {
        std::string argument(argv[i]);
        if(argument.rfind(opt_use_stdout, 0) == 0 && argument.length() == sizeof(opt_use_stdout)) {
            use_stdout = true;
        }
#ifdef VM_DEBUG
        else if(argument.rfind(opt_use_stdin, 0) == 0 && argument.length() == sizeof(opt_use_stdin)) {
            use_stdin = true;
        }
#endif
        else if(!found_filename) {
            found_filename = true;
            filename = argument;
        } else {
            print_usage();
        }
    }

    if(!found_filename) print_usage();

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

    parse_arguments(argc, argv, filename, outFile);

    std::fstream inFile;
    inFile.open("stdin.txt", std::fstream::in);
    std::cout << "Using \"stdin.txt\" as input" << std::endl;

    std::cout << "Starting VM\nLoading program from " << filename << std::endl;
    ToyVM::VM vm(readFile(filename));

    // register default lambdas for input / output operation (port 0)
    vm.registerInput( [&inFile]() {
        word_t word = 0;
        if(inFile.good() && !inFile.eof()) {
            std::string line;
            std::getline(inFile, line);

            std::remove_if(line.begin(), line.end(), isspace);

            if (line.length() != 0)
                word = str_to_i(line);
        }
        return word;
    });

    vm.registerOutput( [&outFile](word_t out) {
        *outFile << out << ' ';
    });
    // register character io (port 1)
    vm.registerInput( [&inFile]() {
        word_t word = 0;
        if(inFile.good() && !inFile.eof()) {
            std::string line;
            std::getline(inFile, line);

            std::remove_if(line.begin(), line.end(), isspace);

            if (line.length() != 0)
                word = str_to_i(line);
        }
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
    inFile.close();
    return 0;
}