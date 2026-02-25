#ifndef AST_H
#define AST_H

#include <cstdint>
#include <memory>
#include <string>

namespace ast {

    struct Node {
        // TODO add more operators later fyrir bonus
        enum class Kind { Number, Add, Mul }; // number + operators[+,-]

        Kind kind;
        std::int64_t value; // used only if Number
        std::unique_ptr<Node> left;
        std::unique_ptr<Node> right;

        static std::unique_ptr<Node> make_number(std::int64_t v);
        static std::unique_ptr<Node> make_op(Kind k, std::unique_ptr<Node> a, std::unique_ptr<Node> b);
    };

    // Build AST from an expression string: 2+7*(1+3)
    std::unique_ptr<Node> parse_expression(const std::string& expr);

    // Convert AST to a string to write to file: (+ 2 (* 7 (+ 1 3)))
    std::string serialize(const Node& root);

    // Read AST back from its serialized string
    std::unique_ptr<Node> deserialize(const std::string& text);

    // Evaluate AST as int64 result
    std::int64_t evaluate(const Node& root);

}

#endif