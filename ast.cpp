#include "ast.h"

#include <cctype> // isdigit, isspace
#include <stdexcept> // runtime_error
#include <sstream> // ostringstream : write to string


using std::unique_ptr;
using std::string;
using std::int64_t;
using std::size_t;




namespace ast {

    // ---------- operator info helpers ----------
    struct OpInfo {
        char symbol;
        Node::Kind kind;
    };

    static const OpInfo ops[] = {
        {'+', Node::Kind::Add},
        {'-', Node::Kind::Sub},
        {'*', Node::Kind::Mul},
        {'/', Node::Kind::Div}
    };

    static bool op_to_kind(char op, Node::Kind& out_kind) {
        for (const auto& info : ops) {
            if (info.symbol == op) {
                out_kind = info.kind;
                return true;
            }
        }
        return false;
    }

    static char kind_to_op(Node::Kind k) {
        for (const auto& info : ops) {
            if (info.kind == k) return info.symbol;
        }
        throw std::runtime_error("serialize: unknown operator kind");
    }

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
            case Node::Kind::Sub:    return evaluate(*root.left) - evaluate(*root.right);
            case Node::Kind::Mul:    return evaluate(*root.left) * evaluate(*root.right);
            case Node::Kind::Div:    {
                int64_t divisor = evaluate(*root.right); // neðri talan í deilingu
                if (divisor == 0) throw std::runtime_error("evaluate: division by zero");
                return evaluate(*root.left) / divisor;
            }
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

    static unique_ptr<Node> parse_rec(string s); // declare for split
    static unique_ptr<Node> try_split(const string& s, char op_char, Node::Kind kind) {
        int k = find_op_at_depth_0(s, op_char);
        if (k == -1) return nullptr;

        if (k == 0) throw std::runtime_error("parse: missing left operand for '" + string(1, op_char) + "' in: " + s);
        if (k == (int) s.size() - 1) throw std::runtime_error("parse: missing right operand for '" + string(1, op_char) + "' in: " + s);

        auto L = parse_rec(s.substr(0, k));
        auto R = parse_rec(s.substr(k + 1));

        return Node::make_op(kind, std::move(L), std::move(R));
    }
    // ---------- parse_expression (build AST: expression -> AST) ----------
    /* precedence order:
        0. lowest
        1. + and -
        2. *
        3. numbers
        4. highest
    */
    static unique_ptr<Node> parse_rec(string s) {
        s = strip_outer_parens(s);
        s = trim_ws(s);

        // try splitting on + or - or * or /
        for (const auto& op : ops) {
            if (auto node = try_split(s, op.symbol, op.kind)) return node;
        }

        // otherwise must be a number
        if (s.empty()) throw std::runtime_error("parse: empty token");

        int64_t v = 0;
        for (char c : s) {
            if (!std::isdigit((unsigned char) c)) throw std::runtime_error("parse: expected number but got: " + s);
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
        char op = kind_to_op(n.kind);
        
        out << "(" << op << " ";
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

                    char op = get(); // '+' 'or '-' or '*'
                    Node::Kind k;
                    if (!op_to_kind(op, k)) throw std::runtime_error("deserialize: expected one of + - * /");

                    skip_ws();
                    auto a = parse_node();
                    skip_ws();
                    auto b = parse_node();
                    skip_ws();
                    if (get() != ')') throw std::runtime_error("deserialize: expected ')'");

                    // create operator node
                    return Node::make_op(k, std::move(a), std::move(b));
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