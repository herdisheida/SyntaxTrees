# SyntaxTrees

PA3 - Syntax Trees

## Compile and run program

1. Compile:

```bash
make
```

2. Run:

The program has two modes:

- **Build mode**: Parse an expression and generate an AST file
- **Evaluation mode**: Read a previously generated AST file and evaluate it

|           | Build Mode                                    | Evaluation Mode                            |
| --------- | --------------------------------------------- | ------------------------------------------ |
| Templates | `./program <ast-file> <expr-file>`            | `./program --eval <ast-file>`              |
|           | `./program <ast-file> (read expr from stdin)` | `./program --eval <ast-file> <var-file>`   |
|           |                                               |                                            |
| Example   | `./program ast.tree tests/sample1.in`         | `./program --eval ast.tree tests/vars.txt` |

- **&lt;ast-file&gt;** : (Build mode) Output file containing the generated AST / (Eval mode) Input file containing the AST to evaluate
- **&lt;expr-file&gt;** : Input file containing the expression to parse
- **&lt;var-file&gt;** : Optional input file defining variable values

### Clean

Remove .o files and the executable:

```bash
make clean
```

## Additional features

1. **Whitespace insensitive** (5%):
   Ignore whitespace characters (spaces, tabs, newlines) in the input expression.
2. **Modern C++ Tree** (5%):
   the AST is implemented like a tree with smart pointers (`std::unique_ptr`)
3. **Extra operations** (10%):
   - binary minus (`-`)
   - binary division (`/`)
   - unary minus

4. **Error handling** (~10%):
   Descriptive error messages for:
   - Files: not found, cannot be opened
   - Invalid expressions: missing operands, unbalanced parentheses, invalid characters
   - Invalid AST file format
   - Division by zero
   - Undefined variables
5. **Variables** (15%):
   Allow reading variables from file. Variable values must be valid 64-bit signed integers
6. **AST optimisations** (~10%):
   while building AST, simplify math for example: 0 and 1 multiplication, 0 and 1 division , 0 addition and subtraction, double negation

## AST file format

AST is stored in a notation similar to Scheme/List format.

Each internal node is written as:

```
(<operator> <left-subtree> <right-subtree>)
```

Unary minus is stored as:

```
(~ <subtree>)
```

Examples:

```cpp
(+ 3 4)  // represents 3 + 4
(* (+ 3 4) 5)  // represents (3 + 4) * 5
(~ 10) // represents -10
```
