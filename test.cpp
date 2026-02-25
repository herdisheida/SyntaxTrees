#include <iostream>
#include <string> // runtime error


class Token {
    /* token types:
    1. number
    2. operator
    3. parenthesis
    */
    public:
        static const char plus = '+';
        static const char minus = '-';
        static const char multiply = '*';
        static const char divide = '/';
        static const char left_paren = '(';
        static const char right_paren = ')';
        
        static const char error = '\0';
};

static const char operators[] = {'+', '-', '*', '/'};


struct SyntaxTreeNode {
    int token;
    SyntaxTreeNode* left;
    SyntaxTreeNode* right;

    // constructor
    SyntaxTreeNode(int val) : token(val), left(nullptr), right(nullptr) {}
    
    // destructor TODO


    /* grammar rules:
    expression = expression + expression
           | expression * expression
           | number
           | (expression)    
    */
    


    void expression() {
        if (is_number()) {
            // number
            
        } else if (token == Token::left_paren) {
            // parenthesized expression
            expression();
            if (token != Token::right_paren) {
                throw std::runtime_error("Expected ')', got: " + std::to_string(token));
            }

        } else {
            // error handling
            throw std::runtime_error("Invalid expression: " + std::to_string(token));
        }
    }

    bool is_number() {
        /* ascii numbers */
        if (token >= '0' && token <= '9') return true;
        return false;
    }
};

