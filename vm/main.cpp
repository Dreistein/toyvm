//
// Dreistein, Feb. 2018
//

#include <iostream>
#include <fstream>
#include <algorithm>

#include "VM.h"

using ToyVM::word_t;

inline word_t str_to_i(const std::string& line) {
    if (line.substr(0, 2) == "0b")
        return static_cast<word_t>(std::stoi(line.substr(2), nullptr, 2));
    else
        return static_cast<word_t>(std::stoi(line, nullptr, 0));
}

inline std::vector<word_t> readFile(const std::string& filename) {
    std::vector<word_t> values;
    std::fstream fs;
    fs.open(filename, std::fstream::in);
    if(fs.good()) {
        while (!fs.eof()) {
            std::string line;
            std::getline(fs, line);

            std::remove_if(line.begin(), line.end(), isspace);

            if (line.length() == 0)
                continue;

            values.push_back(str_to_i(line));
        }
    }
    fs.close();
    return values;
}

int main(int argc, const char *argv[]) {

    if (argc != 2) {
        std::cout << "Usage: vm <program>" << std::endl;
        return 0;
    }

    std::cout << "Started VM\nLoading program from " << argv[1] << std::endl;
    ToyVM::VM vm(readFile(argv[1]));


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

    vm.run();

    // close output stream
    stdoutFile.close();
    // wait to show output before exiting
    std::system("pause");
    return 0;
}