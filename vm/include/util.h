
#pragma once

#include <vector>
#include <string>
#include "definitions.h"

std::vector<ToyVM::word_t> readFile(const std::string& filename);
ToyVM::word_t str_to_i(const std::string& line);