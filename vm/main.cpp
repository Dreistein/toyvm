//
// Dreistein, Feb. 2018
//

#include <iostream>
#include <fstream>
#include <algorithm>

#include "VM.h"

using ToyVM::word_t;

std::vector<word_t> loadProgram(const std::string& filename) {

    std::vector<word_t> program;
    std::fstream fs;
    fs.open(filename, std::fstream::in);

    while (!fs.eof())
    {
        word_t word;
        std::string line;
        std::getline(fs, line);

        std::remove_if(line.begin(), line.end(), isspace);

        if(line.length() == 0)
            continue;

        if(line.substr(0,2) == "0b") { // binary representation
            word = static_cast<word_t>(std::stoi(line.substr(2),0,2));
        }
        else if (line.substr(0,2) == "0x") { // hex representation
            word = static_cast<word_t>(std::stoi(line.substr(2),0,16));
        }
        else { // standard integer
            word = static_cast<word_t>(std::stoi(line));
        }
        program.push_back(word);
    }

    fs.close();
    return program;
}

int main(int argc, const char *argv[]) {

    if (argc != 2) {
        std::cout << "Usage: vm <program>" << std::endl;
        return 0;
    }

    auto program = loadProgram(argv[1]);

    ToyVM::VM vm(program);

    // register default lambdas for input / output oeration (port 0)
    vm.registerInput( []() { word_t input; std::cin >> input; return input; });
    vm.registerOutput( [](word_t out) {
        std::cout << out << std::endl;
    });

    vm.run();

    // wait to show output before exiting
    std::system("pause");
    return 0;
}