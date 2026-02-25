#include "ast.h"
#include <iostream>
#include <string>
#include <fstream> // file streams


using std::cerr; // console error output
using std::cout; // console output
using std::cin;  // console input

using std::endl; // newline
using std::string;

using std::istream;  // input stream
using std::ifstream; // file input stream
using std::ofstream; // file output stream



static string read_stdin(std::istream& in) {
    /* read from stdin: ignoring whitespace, newline, tab, etc */
    string result;
    char c;
    while (in >> c)
        result += c;
    return result;
}

static string read_file(const char* filename) {
    /* read from file: ignoring whitespace, newline, tab, etc */
    try {
        ifstream file(filename);
        if (!file) throw std::runtime_error("Could not open file: " + string(filename));

        // read file char by char
        string result;
        char c;
        while (file.get(c)) {
            if (std::isspace((unsigned char)c)) continue;
            result += c;
        }
        file.close();
        return result;
    } catch (const std::exception& e) {
        cerr << "Error: " << e.what() << endl;
        return "";
    }
}


void print_usage(const char* program_name) {
    cerr << "Usage:" << endl << endl;
    cerr << "  " << "Build AST:" << endl;
    cerr << "    " << program_name << " <ast-out-file> <expr-in-file>" << endl;
    cerr << "    " << program_name << " <ast-out-file> (read expr from stdin)" << endl;
    cerr << "  " << "Evaluate AST:" << endl;
    cerr << "    " << program_name << " --eval <ast-file>" << endl;
}

void evaluate_AST(const char* filename) {
    string ast_input = read_file(filename);
    cout << "AST read from file (" << filename << "): " << ast_input << endl;
    auto root = ast::deserialize(ast_input);
    cout << "Result: " << ast::evaluate(*root) << endl;
}

int main(int argc, char** argv) {
    try {
        if (argc != 2 && argc != 3) {
            // incorrect num of args
            print_usage(argv[0]);
            return 1;
        }

        if (argc == 3 && string(argv[1]) == "--eval") {
            evaluate_AST(argv[2]);
            return 0;
        }

        // build AST mode:
        // 1. get input expr
        string input;
        if (argc == 3) {
            // get expr from file
            input = read_file(argv[2]);
            cout << "Input read from file (" << argv[2] << "): " << input << endl;
        }
        if (argc == 2) {
            // get expr from stdin
            input = read_stdin(cin);
            cout << "Input read from stdin: " << input << endl;
        }

        // 2. build AST from input expr
        auto root = ast::parse_expression(input);
        string ast_representation = ast::serialize(*root);

        // 3. write to argv[1] (ast output file)
        ofstream output_file(argv[1]);
        if (!output_file) throw std::runtime_error("Could not open output file: " + string(argv[1]));
        output_file << ast_representation << endl;
        output_file.close();
        return 0;
    } catch (const std::exception& e) {
        cerr << "Error: " << e.what() << endl;
        return 1;
    }
}
