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


 /* read from stdin: ignoring whitespace, newline, tab, etc */
static string read_stdin(std::istream& in) {
    string result;
    char c;
    while (in >> c)
        result += c;

    if (in.bad()) throw std::runtime_error("Error while reading from stdin");
    if (result.empty()) throw std::runtime_error("No input received from stdin");
    return result;
}

/* read from file: ignoring whitespace, newline, tab, etc */
static string read_file(const char* filename, bool ignore_whitespace = true) {
    ifstream file(filename);
    if (!file) throw std::runtime_error("Could not open file: " + string(filename));

    // read file char by char
    string result;
    char c;
    while (file.get(c)) {
        if (ignore_whitespace && std::isspace((unsigned char)c)) continue;
        result += c;
    }
    if (!file.eof()) throw std::runtime_error("Error while reading file: " + string(filename));
    file.close();
    return result;
}

void evaluate_AST(const char* filename) {
    string ast_input = read_file(filename, false);  // do not ignore whitespace for AST input
    cout << "AST read from file (" << filename << "): " << ast_input << endl;
    
    auto root = ast::deserialize(ast_input);
    auto result = ast::evaluate(*root);
    cout << "Result: " << result << endl;
}

void print_usage(const char* program_name) {
    cerr << "Usage:" << endl << endl;
    cerr << "  " << "Build AST:" << endl;
    cerr << "    " << program_name << " <ast-out-file> <expr-in-file>" << endl;
    cerr << "    " << program_name << " <ast-out-file> (read expr from stdin)" << endl;
    cerr << "  " << "Evaluate AST:" << endl;
    cerr << "    " << program_name << " --eval <ast-file>" << endl;
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

        // ---------- build AST mode ----------
        // 1. get expression string
        string input;
        if (argc == 3) input = read_file(argv[2]); // read from file
        if (argc == 2) input = read_stdin(cin); // read from stdin

        // 2. build AST from input expr
        auto root = ast::parse_expression(input);
        string ast_str_representation = ast::serialize(*root);

        // 3. write AST to output file
        ofstream output_file(argv[1]);
        if (!output_file) throw std::runtime_error("Could not open output file: " + string(argv[1]));
        output_file << ast_str_representation << endl;
        output_file.close();
        cout << "AST written to file (" << argv[1] << "): " << ast_str_representation << endl;
        return 0;
    } catch (const std::exception& e) {
        cerr << "Error: " << e.what() << endl;
        return 1;
    }
}
