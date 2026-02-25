#ifndef AST_H
#define AST_H

#include <cstdint>
#include <memory>
#include <string>
#include <unordered_map>

using VarMap = std::unordered_map<std::string, std::int64_t>;


namespace ast {

    struct Node {
        // variable, number, adding, subtracting, multiplying, dividing, negative
        enum class Kind { Var, Number, Add, Sub, Mul, Div, Neg };

        Kind kind;

        std::int64_t value; // used for Number
        std::string name;   // used for Var

        std::unique_ptr<Node> left;
        std::unique_ptr<Node> right;

        static std::unique_ptr<Node> make_var(const std::string& name);
        static std::unique_ptr<Node> make_number(std::int64_t v);
        static std::unique_ptr<Node> make_op(Kind k, std::unique_ptr<Node> a, std::unique_ptr<Node> b);
    };

    // Build AST from an expression string, like 2+7*(1+3)
    std::unique_ptr<Node> parse_expression(const std::string& expr);

    // Convert AST to a string to write to file, becomes like (+ 2 (* 7 (+ 1 3)))
    std::string serialize(const Node& root);

    // Read AST back from its serialized string
    std::unique_ptr<Node> deserialize(const std::string& text);

    // Evaluate AST as int64 result
    std::int64_t evaluate(const Node& root, const VarMap& env);

}

#endif