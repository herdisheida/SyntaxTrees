#include "ast.h"

#include <cctype> // isdigit, isspace
#include <stdexcept> // runtime_error
#include <sstream> // ostringstream : write to string
#include <unordered_map>

#include <iostream> // debug


using std::unique_ptr;
using std::string;
using std::int64_t;
using std::size_t;

using VarMap = std::unordered_map<std::string, std::int64_t>;


namespace ast {

    // ---------- operator info helpers ----------
    struct OpInfo {
        char symbol;
        Node::Kind kind;
    };

    static const OpInfo ops[] = {
        {'+', Node::Kind::Add}, // plus
        {'-', Node::Kind::Sub}, // binary minus
        {'*', Node::Kind::Mul}, // multiply
        {'/', Node::Kind::Div}, // divide (round down)
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
    static bool is_number(const unique_ptr<Node>& n, int64_t v) {
        return n && n -> kind == Node::Kind::Number && n -> value == v;
    }

    static bool is_number_node(const unique_ptr<Node>& n) {
        return n && n -> kind == Node::Kind::Number;
    }


    unique_ptr<Node> Node::make_var(const string& var_name) {
        auto n = std::make_unique<Node>();
        n -> kind = Kind::Var;
        n -> value = 0; // not used
        n -> name = var_name;
        return n;
    }

    unique_ptr<Node> Node::make_number(int64_t v) {
        auto n = std::make_unique<Node>();
        n -> kind = Kind::Number;
        n -> value = v;
        return n;
    }

    unique_ptr<Node> Node::make_op(Kind k, unique_ptr<Node> a, unique_ptr<Node> b) {
        // ---------- AST Optimisations ----------
        if (k == Kind::Mul) {
            if (is_number(a, 0) || is_number(b, 0)) return make_number(0);
            if (is_number(a, 1)) return b;
            if (is_number(b, 1)) return a;
        }
        if (k == Kind::Div) {
            if (is_number(a, 0)) return make_number(0);
            if (is_number(b, 1)) return a;
        }
        if (k == Kind::Add) {
            if (is_number(a, 0)) return b;
            if (is_number(b, 0)) return a;
        }
        if (k == Kind::Sub) {
            if (is_number(b, 0)) return a;
        }

        // ---------- make_number right away ----------
        if (is_number_node(a) && is_number_node(b)) {
            if (k == Kind::Add) return make_number(a -> value + b -> value);
            if (k == Kind::Sub) return make_number(a -> value - b -> value);
            if (k == Kind::Mul) return make_number(a -> value * b -> value);
            if (k == Kind::Div) {
                if (b -> value == 0) throw std::runtime_error("Build AST: division by zero");
                return make_number(a -> value / b -> value);
            }
        }
 
        // ---------- Build Normal Node ----------
        auto n = std::make_unique<Node>();
        n -> kind = k;
        n -> value = 0; // not used
        n -> left = std::move(a);   // a becomes owned by n
        n -> right = std::move(b);  // b becomes owned by n
        return n;
    }


    // ---------- evaluate ----------
    int64_t evaluate(const Node& root, const VarMap& env) {
        switch (root.kind) {
            case Node::Kind::Number: return root.value;
            case Node::Kind::Add:    return evaluate(*root.left, env) + evaluate(*root.right, env);
            case Node::Kind::Sub:    return evaluate(*root.left, env) - evaluate(*root.right, env);
            case Node::Kind::Mul:    return evaluate(*root.left, env) * evaluate(*root.right, env);
            case Node::Kind::Div:    {
                int64_t divisor = evaluate(*root.right, env); // neðri talan í almennu broti
                if (divisor == 0) throw std::runtime_error("evaluate: division by zero");
                return evaluate(*root.left, env) / divisor;
            }
            case Node::Kind::Neg:    return -evaluate(*root.left, env);
            case Node::Kind::Var: {
                auto it = env.find(root.name);
                if (it == env.end()) throw std::runtime_error("evaluate: undefined variable: " + root.name);
                return it -> second;
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

    /*
        if the expr string is wrapped by one outer (...), remove it.
        Keep stripping until no more outer parens can be removed.
        e.g. (((x))) -> x
    */
    static string strip_outer_parens(const string& s) {
        string t = trim_ws(s);
        
        while (true) {
            t = trim_ws(t);
            if (t.size() < 2 || t.front() != '(' || t.back() != ')') return t; // not wrapped by parens

            int depth = 0;
            bool fully_wrapped = true;

            for (size_t i = 0; i < t.size(); i++) {
                // track parentheses depth
                if (t[i] == '(') depth++;
                else if (t[i] == ')') depth--;

                // parens closes early (not at last char), not a full wrapper
                if (depth == 0 && i != t.size() - 1) {
                    fully_wrapped = false;
                    break;
                }
        
                if (depth < 0) throw std::runtime_error("parse strip_outer_parens: unbalanced parentheses in: " + t);
            }
            if (depth != 0) throw std::runtime_error("parse strip_outer_parens: unbalanced parentheses in: " + t);
            
            if (!fully_wrapped) return t; // have removed ALL wrapperd parens

            // strip one wrapper and continue
            t = trim_ws(t.substr(1, t.size() - 2));
        }
    }

    /* find last operator at parentheses depth 0  (left-associative operators) */
    static int find_last_op_at_depth_0(const string& s, char op) {
        int depth = 0;
        int result = -1;
    
        for (int i = 0; i < (int) s.size(); i++) {
            char c = s[i];
            if (c == '(') {
                depth++;
            } else if (c == ')') {
                depth--;
            } else if (depth == 0 && c == op) {
                // skip unary minus
                if (op == '-' && (i == 0 || s[i-1] == '(' || s[i-1] == '+' || s[i-1] == '-' || s[i-1] == '*' || s[i-1] == '/')) continue;
                result = i; // last occurance of op found at depth 0
            }
        }
        return result; // not found
    }

    static unique_ptr<Node> parse_rec(string s); // declare for split
    static unique_ptr<Node> try_split(const string& s, char op_char, Node::Kind kind) {
        int k = find_last_op_at_depth_0(s, op_char);
        if (k == -1) return nullptr;

        if (k == 0) throw std::runtime_error("parse: missing left operand for '" + string(1, op_char) + "' in: " + s);
        if (k == (int) s.size() - 1) throw std::runtime_error("parse: missing right operand for '" + string(1, op_char) + "' in: " + s);

        auto L = parse_rec(s.substr(0, k));
        auto R = parse_rec(s.substr(k + 1));

        return Node::make_op(kind, std::move(L), std::move(R));
    }
    // ---------- parse_expression (build AST: expression -> AST) ----------
    /* precedence order:
        1. unary minnus
        2. + and -
        3. * and /
        4. numbers
    */
    static unique_ptr<Node> parse_rec(string s) {
        s = strip_outer_parens(s);
        s = trim_ws(s);

        // handle binary operators [+ - * /]
        for (const auto& op : ops) {
            if (auto node = try_split(s, op.symbol, op.kind)) return node;
        }
        if (s.empty()) throw std::runtime_error("parse: empty token");

        // handle unary minus
        if (s[0] == '-') {
            auto operand = parse_rec(s.substr(1));
            // double negation
            if (operand && operand -> kind == Node::Kind::Neg) return std::move(operand -> left); // -x becomes x
            
            return Node::make_op(Node::Kind::Neg, std::move(operand), unique_ptr<Node>{});
        }

        // variable: all lowercase letters
        bool all_lower = true;
        for (char c : s) {
            if (!(c >= 'a' && c <= 'z')) { all_lower = false; break; }
        }
        if (all_lower) return Node::make_var(s);

        // number: all digits
        bool all_digits = true;
        for (char c : s) {
            if (!std::isdigit((unsigned char) c)) { all_digits = false; break; }
        }
        if (all_digits) {
            int64_t v = 0;
            for (char c : s) v = v * 10 + (c - '0');  // string -> ascii -> digit
            return Node::make_number(v);
        }

        throw std::runtime_error("parse: expected number or variable but got: " + s);
    }

    unique_ptr<Node> parse_expression(const string& expr) {
        return parse_rec(expr);
    }

    // ---------- serialize (AST -> string) ----------
    static void serialize_rec(std::ostream& out, const Node& n) {
        if (n.kind == Node::Kind::Number) { out << n.value; return; }
        if (n.kind == Node::Kind::Var)    { out << n.name;  return; }

        // handle unary minus
        if (n.kind == Node::Kind::Neg) {
            out << "(~ ";
            serialize_rec(out, *n.left);
            out << ")";
            return;
        }

        char op = kind_to_op(n.kind); // binary operator
                
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

                    char op = get(); // binary operators
                    if (op == '~') {
                        skip_ws();
                        auto a = parse_node();
                        skip_ws();
                        const char closing = get();
                        if (closing != ')') throw std::runtime_error("deserialize: expected ')' for unary operator at pos " + std::to_string(pos_));
                        return Node::make_op(Node::Kind::Neg, std::move(a), unique_ptr<Node>{});
                    }

                    Node::Kind k;
                    if (!op_to_kind(op, k)) throw std::runtime_error("deserialize: expected one of + - * / but got '" + string(1, op) + "'");

                    skip_ws();
                    auto a = parse_node();
                    skip_ws();
                    auto b = parse_node();
                    skip_ws();

                    const char closing = get();
                    if (closing != ')') throw std::runtime_error("deserialize: expected ')' but got '" + string(1, closing) + "'");

                    // create operator node
                    return Node::make_op(k, std::move(a), std::move(b));
                }

  
                // number or variable
                if (std::isdigit((unsigned char) peek())) return Node::make_number(parse_number());
                if (peek() >= 'a' && peek() <= 'z') return Node::make_var(parse_identifier());
                
                throw std::runtime_error("deserialize: expected number or variable but got '" + string(1, peek()) + "'");
            }

            string parse_identifier() {
                // parse variable
                skip_ws();
                string name;
                while (pos_ < s_.size()) {
                    char c = peek();
                    if (c >= 'a' && c <= 'z') {
                        name += get();
                    } else {
                        break;
                    }
                }
                if (name.empty())
                    throw std::runtime_error("deserialize: expected variable but got '" + string(1, peek()) + "'");
                return name;
            }

            int64_t parse_number() {
                skip_ws();
                bool any = false;
                int64_t v = 0;
                while (std::isdigit((unsigned char) peek())) {
                    any = true;
                    v = v * 10 + (get() - '0');
                }
                if (!any) throw std::runtime_error("deserialize: expected number but got '" + string(1, peek()) + "'");
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