#include "ast.h"

#include <cctype> // isdigit, isspace
#include <stdexcept> // runtime_error
#include <sstream> // ostringstream : write to string


using std::unique_ptr;
using std::string;
using std::int64_t;
using std::size_t;

namespace ast {

    // ---------- build Nodes (operators and numbers) ----------
    unique_ptr<Node> Node::make_number(int64_t v) {
        auto n = std::make_unique<Node>();
        n -> kind = Kind::Number;
        n -> value = v;
        return n;
    }

    unique_ptr<Node> Node::make_op(Kind k, unique_ptr<Node> a, unique_ptr<Node> b) {
        auto n = std::make_unique<Node>();
        n -> kind = k;
        n -> value = 0; // not used for operators
        n -> left = std::move(a);   // a becomes owned by n
        n -> right = std::move(b);  // b becomes owned by n
        return n;
    }

    // ---------- evaluate ----------
    int64_t evaluate(const Node& root) {
        switch (root.kind) {
            case Node::Kind::Number: return root.value;
            case Node::Kind::Add:    return evaluate(*root.left) + evaluate(*root.right);
            case Node::Kind::Mul:    return evaluate(*root.left) * evaluate(*root.right);
        }
        throw std::runtime_error("evaluate: unknown node kind");
    }

    // ---------- utilities ----------
    /* remove whitespace at beginning and end of string */
    static string trim_ws(const string& s) {
        size_t i = 0, j = s.size();
        while (i < j && std::isspace((unsigned char) s[i])) i++;
        while (j > i && std::isspace((unsigned char) s[j - 1])) j--;
        return s.substr(i, j - i);
    }

    /* if the expr string is wrapped by one outer (...), remove it */
    static string strip_outer_parens(const string& s) {
        string t = trim_ws(s);
        
        if (t.size() < 2 || t.front() != '(' || t.back() != ')') return t;

        int depth = 0;
        for (size_t i = 0; i < t.size(); i++) {
            if (t[i] == '(') depth++;
            else if (t[i] == ')') depth--;
            if (depth == 0 && i != t.size() - 1) {
                return t; // parens closes early, not a full wrapper
            }
        }
        // fully wrapped
        return trim_ws(t.substr(1, t.size() - 2));
    }

    /* find an operator at parentheses depth 0 */
    static int find_op_at_depth_0(const string& s, char op) {
        int depth = 0;
        for (int i = 0; i < (int) s.size(); i++) {
            char c = s[i];
            if (c == '(') depth++;
            else if (c == ')') depth--;
            else if (depth == 0 && c == op) return i; // op found at depth 0
        }
        return -1; // not found
    }

    // ---------- parse_expression (build AST: expression -> AST) ----------
    /* precedence order:
        0. lowest
        1. +
        2. *
        3. numbers
        4. highest
    */
    static unique_ptr<Node> parse_rec(string s) {
        s = strip_outer_parens(s);
        s = trim_ws(s);

        // split on '+'
        int k = find_op_at_depth_0(s, '+'); // check lower precedence first
        if (k != -1) {  // '+' found
            auto L = parse_rec(s.substr(0, k));
            auto R = parse_rec(s.substr(k + 1));
            return Node::make_op(Node::Kind::Add, std::move(L), std::move(R));
        }

        // then split on '*'
        k = find_op_at_depth_0(s, '*');
        if (k != -1) {  // '*' found
            auto L = parse_rec(s.substr(0, k));
            auto R = parse_rec(s.substr(k + 1));
            return Node::make_op(Node::Kind::Mul, std::move(L), std::move(R));
        }

        // otherwise must be a number
        if (s.empty()) throw std::runtime_error("parse: empty token");

        int64_t v = 0;
        for (char c : s) {
            if (!std::isdigit((unsigned char) c))
                throw std::runtime_error("parse: expected number but got: " + s);
            v = v * 10 + (c - '0');  // string -> ascii -> digit
        }
        return Node::make_number(v);
    }

    unique_ptr<Node> parse_expression(const string& expr) {
        return parse_rec(expr);
    }

    // ---------- serialize (AST -> string) ----------
    static void serialize_rec(std::ostream& out, const Node& n) {
        if (n.kind == Node::Kind::Number) {
            out << n.value;
            return;
        }
        out << "(" << (n.kind == Node::Kind::Add ? "+" : "*") << " ";
        serialize_rec(out, *n.left);
        out << " ";
        serialize_rec(out, *n.right);
        out << ")";
    }

    string serialize(const Node& root) {
        std::ostringstream out;
        serialize_rec(out, root);
        return out.str();
    }


    // ---------- deserialize (string -> AST) ----------
    class AstTextParser {
        public:
            // constructor
            explicit AstTextParser(string s) { s_ = std::move(s); }

            unique_ptr<Node> parse_all() {
                skip_ws();
                auto n = parse_node();
                skip_ws();
                if (pos_ != s_.size()) {
                    throw std::runtime_error("deserialize: trailing characters at pos " + std::to_string(pos_));
                }
                return n;
            }

        private:
            // class variables
            string s_;
            size_t pos_ = 0;


            /* expects (op num num) */
            unique_ptr<Node> parse_node() {
                skip_ws();
                if (peek() == '(') {
                    get(); // '('
                    skip_ws();
                    char op = get(); // '+' or '*'
                    if (op != '+' && op != '*') throw std::runtime_error("deserialize: expected + or *");
                    skip_ws();
                    auto a = parse_node();
                    skip_ws();
                    auto b = parse_node();
                    skip_ws();
                    if (get() != ')') throw std::runtime_error("deserialize: expected ')'");
                    return Node::make_op(op == '+' ? Node::Kind::Add : Node::Kind::Mul, std::move(a), std::move(b));
                }
                return Node::make_number(parse_number());
            }

            int64_t parse_number() {
                skip_ws();
                bool any = false;
                int64_t v = 0;
                while (std::isdigit((unsigned char) peek())) {
                    any = true;
                    v = v * 10 + (get() - '0');
                }
                if (!any) throw std::runtime_error("deserialize: expected number at pos " + std::to_string(pos_));
                return v;
            }

            /* skips whitespace */
            void skip_ws() {
                while (pos_ < s_.size() && std::isspace((unsigned char) s_[pos_])) pos_++;
            }
            /* look at curr character */
            char peek() const { return pos_ < s_.size() ? s_[pos_] : '\0'; }
            /* return curr char and increment pos_ */
            char get() { return pos_ < s_.size() ? s_[pos_++] : '\0'; }
        };

        unique_ptr<Node> deserialize(const string& text) {
            AstTextParser p(text);
            return p.parse_all();
        }
}