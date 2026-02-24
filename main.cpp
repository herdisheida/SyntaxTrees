// #include "ast.h"
#include <iostream>
#include <string>

using std::cerr; // console error output
using std::cout; // console output
using std::endl; // newline
using std::string;

static std::string read_all(std::istream& in) {
    // TODO: read all input from stream, return as string
    return "NULL"; // placeholder
}

void print_usage(const char* program_name) {
    cerr << "Usage:" << endl;
    cerr << "  " << program_name << " <ast_out_file> <expr_in_file>" << endl;
    cerr << "  " << program_name << " --eval <ast_file>" << endl;
}

int main(int argc, char** argv) {
    try {
        /*
        build AST mode (write to file):
            1. ./<program-name>   <ast-output-file>   <expr-input-file>
            2. ./<program-name>   <ast-output-file>   (read from stdin)
        evaluate AST mode (read from file):
            1. ./<program-name>   --eval   <ast-file>
        */

        if (argc != 2 && argc != 3) {
            // incorrect num of args
            print_usage(argv[0]);
            return 1;
        }

        if (string(argv[1]) == "--eval") {
            if (argc == 3) {
                // evaluate AST mode
                // TODO: read from file, evaluate AST, print result
                // 1. read from argv[2]
                // 2. evaluate AST (using syntax tree structure) - todo búa til structureið
                // 3. print result
                return 0;
            } else {
                // incorrect num of args for eval mode
                print_usage(argv[0]);
                return 1;
            }
        }

        // build AST mode
        if (argc == 3) {  // get expr from file
            // TODO: read from file, build AST, write to file
            // 1. read from argv[2] (expr input file)
            // 2. build AST (using syntax tree structure) - todo búa til structureið
            // 3. write to argv[1] (ast output file)
        }

        if (argc == 2) {  // get expr from stdin
            std::string input = read_all(std::cin);
            // TODO: build AST from input, write to argv[1]
            // 1. read input from stdin
            // 2. build AST (using syntax tree structure) - todo búa til structureið
            // 3. write to argv[1] (ast output file)
        }


        return 0;
    } catch (const std::exception& e) {
        cerr << "Error: " << e.what() << endl;
        return 1;
    }
}
