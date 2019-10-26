
#include "util.h"

#include <iostream>
#include <fstream>
#include <algorithm>

using ToyVM::word_t;

std::vector<word_t> readFile(const std::string& filename) {
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

// converts a number literal to a word
ToyVM::word_t str_to_i(const std::string& line) {
    if (line.substr(0, 2) == "0b")
        return static_cast<ToyVM::word_t>(std::stoi(line.substr(2), nullptr, 2));
    else
        return static_cast<ToyVM::word_t>(std::stoi(line, nullptr, 0));
}