
#include <string>
#include <iostream>

//#include 

namespace ToyVM {

    class OutputWriter {

    private:
        bool use_stdout;
        std::fstream output_stream;

    public:
        inline OutputWriter() : use_stdout(true) {}
        inline OutputWriter(std::string file) : use_stdout(false) {
            output_stream.open(file, std::fstream::out);
        }

        inline ~OutputWriter() {
            if(!use_stdout) output_stream.close();
        }

        inline bool good() {
            return use_stdout || output_stream.good();
        }

        inline std::ostream& operator<< (std::ostream& (*pf)(std::ostream&)) {
            if(use_stdout)
                return std::cout << pf;
            return output_stream << pf;
        }

        inline std::ostream& operator<< (word_t word) {
            if(use_stdout)
                return std::cout << word;
            return output_stream << word;   
        }

        inline std::ostream& operator<< (char word) {
            if(use_stdout)
                return std::cout << word;
            return output_stream << word;   
        }
        
    };

}