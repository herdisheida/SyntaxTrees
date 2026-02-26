#include "ast.h"
#include <cstddef>
#include <iostream>
#include <string> // string, stoll, etc
#include <fstream> // file streams
#include <unordered_map> // for variable environment


using std::cerr; // console error output
using std::cout; // console output
using std::cin;  // console input

using std::endl; // newline
using std::string;

using std::istream;  // input stream
using std::ifstream; // file input stream
using std::ofstream; // file output stream

using VarMap = std::unordered_map<std::string, std::int64_t>; // unordered map for variables (name to value)


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
static string read_file(const char* filename, bool ignore_whitespace = true, const string& file_description = "file") {
    ifstream file(filename);
    if (!file) throw std::runtime_error("Could not open " + file_description + ": " + string(filename));

    // read file char by char
    string result;
    char c;
    while (file.get(c)) {
        if (ignore_whitespace && std::isspace((unsigned char)c)) continue;
        result += c;
    }
    if (!file.eof()) throw std::runtime_error("Error while reading " + file_description + ": " + string(filename));
    file.close();
    return result;
}

static VarMap read_vars_file(const char* filename) {
    ifstream file(filename);
    if (!file) throw std::runtime_error(std::string("Could not open variable file: ") + filename);

    VarMap env;
    string line;

    while (std::getline(file, line)) {
        // remove spaces/tabs
        string s;
        for (char c : line) if (!std::isspace((unsigned char)c)) s += c;
        if (s.empty()) continue;

        auto eq = s.find('=');
        if (eq == string::npos) throw std::runtime_error("vars: expected 'name=value' but got: " + s);
        
        string name = s.substr(0, eq);
        string value_str = s.substr(eq + 1);

        // valid name: all lowercase letters
        if (name.empty()) throw std::runtime_error("vars: empty name in line: " + s);
        for (char c : name) if (!(c >= 'a' && c <= 'z')) throw std::runtime_error("vars: invalid variable name: " + name);

        // valid value: all digits
        for (char c : value_str) {
            // consecutive minuses not allowed, but single leading allowed (for unary minus)
            if (c == '-' && strcmp(&c, &value_str[0])) continue;
            if (!std::isdigit((unsigned char) c)) throw std::runtime_error("vars: invalid variable value: " + value_str);
        }
        
        if (value_str.empty()) throw std::runtime_error("vars: empty value in line: " + s);

        int64_t value = std::stoll(value_str);  // convert string into int64_t
        env[name] = value;
    }
    return env;
}


void evaluate_AST(const char* filename, const char* vars_filename = NULL) {
    string ast_input = read_file(filename, false, "AST file");  // do not ignore whitespace for AST input
    if (ast_input.empty()) throw std::runtime_error("AST input file is empty: " + string(filename));

    VarMap env;
    if (vars_filename != NULL) env = read_vars_file(vars_filename);
    
    auto root = ast::deserialize(ast_input);
    auto result = ast::evaluate(*root, env);
    cout << "Result: " << result << endl;
}

void print_usage(const char* program_name) {
    cerr << "Usage:" << endl << endl;
    cerr << "  " << "Build AST:" << endl;
    cerr << "    " << program_name << " <ast-file> <expr-file>" << endl;
    cerr << "    " << program_name << " <ast-file> (read expr from stdin)" << endl;
    cerr << "  " << "Evaluate AST:" << endl;
    cerr << "    " << program_name << " --eval <ast-file>" << endl;
    cerr << "    " << program_name << " --eval <ast-file> <vars-file>" << endl;

}


int main(int argc, char** argv) {
    try {
        if (argc != 2 && argc != 3 && argc != 4) {
            // incorrect num of args
            print_usage(argv[0]);
            return 1;
        }

        if (string(argv[1]) == "--eval") {
            // normal eval mode: --eval <ast-file>
            if (argc == 3) {
                evaluate_AST(argv[2]);
                return 0;
            }
            
            // eval mode with variables: --eval <ast-file> <vars-file>
            else if (argc == 4) {
                evaluate_AST(argv[2], argv[3]);
                return 0;
            }

            else {
                print_usage(argv[0]);
                return 1;
            }
        }

        // ---------- build AST mode ----------
        // 1. get expression string
        string input;
        if (argc == 3) input = read_file(argv[2], true, "input expression file"); // read from file
        if (argc == 2) input = read_stdin(cin); // read from stdin
        if (input.empty()) throw std::runtime_error("Could not read input expression (empty input)");

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
