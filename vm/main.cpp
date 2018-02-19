#include <iostream>
#include <fstream>

#include "VM.h"

std::vector<word_t> loadProgram(const std::string& filename) {
    std::vector<word_t> program;
    std::fstream fs;
    fs.open(filename, std::fstream::in);

    while (!fs.eof())
    {
        word_t word;
        std::string line;
        std::getline(fs, line);
        if(line.substr(0,2) == "0b") {
            word = static_cast<word_t>(std::stoi(line.substr(2),0,2));
        }
        else if (line.substr(0,2) == "0x") {
            word = static_cast<word_t>(std::stoi(line.substr(2),0,16));
        } else {
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

    VM vm(program);

    vm.registerInput( []() { word_t input; std::cin >> input; return input; });
    vm.registerOutput( [](word_t out) {
        std::cout << out << std::endl;
    });

    vm.run();

    std::system("pause");
    return 0;
}