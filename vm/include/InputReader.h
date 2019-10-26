
#include <string>
#include <iostream>

#include "util.h"

namespace ToyVM {

    class InputReader {

    private:
        bool use_stdin;
        std::fstream input_stream;

    public:
        inline InputReader() : use_stdin(true) {}
        inline InputReader(std::string file) : use_stdin(false) {
            input_stream.open(file, std::fstream::in);
        }

        inline ~InputReader() {
            if(!use_stdin) input_stream.close();
        }

        inline bool good() {
            return use_stdin || input_stream.good();
        }

        inline bool eof() {
            if(use_stdin) {
                return std::cin.eof();
            }
            return input_stream.eof();
        }

        inline std::istream& operator>> (std::istream& (*pf)(std::istream&)) {
            if(use_stdin)
                return std::cin >> pf;
            return input_stream >> pf;
        }

        inline std::istream& operator>> (std::string& line) {
            if(use_stdin) {
                std::cout << "<Input Requested>:" <<std::endl;
                std::getline(std::cin, line);
                return std::cin;
            }
            std::getline(input_stream, line);
            return input_stream;
        }

        inline std::istream& operator>> (word_t& word) {

            word = 0;
            if(good() && !eof()) {
                std::string line;
                *this >> line;

                std::remove_if(line.begin(), line.end(), isspace);

                if (line.length() != 0)
                    word = str_to_i(line);
            }

            if(use_stdin)
                return std::cin;
            return input_stream;   
        }

        inline std::istream& operator>> (sword_t& sword) {
            word_t word;
            *this >> word;
            sword = static_cast<sword_t>(word);

            if(use_stdin)
                return std::cin;
            return input_stream;    
        }
        
    };

}