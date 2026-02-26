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

TODO--- klara skyldu stuff í README.md

[x] What needs to be written in the terminal to compile and run the program, explaining how to run each of the two parts.

[ ] What additional features were implemented above the base version, if any.

1. Whitespace insensitive (5%): ignore whitespace characters (spaces, tabs, newlines) in the input expression.
2. Modern C++ Tree (5%): the AST is implemented like a tree (left and right children) with smart pointers (std::unique_ptr)
3. Extra operations (10%): support binary minus, binary division and unary minus
4. Error handling (~10%): appropriate error msg - file not found, invalid expression (e.g., missing parentheses, invalid characters, etc), invalid AST file format, division by zero
5. Variables (15%): allow reading variables from file, variable value has to be a valid integer
6. AST optimisations (~10%): while building AST pre-calculate: 0 and 1 multiplication, 0 and 1 division , 0 addition and subtraction, double negation

[ ] An explanation of the format you store the AST (Abstract Syntax Tree) in that the two versions read/write.

[ ] Anything else a student wishes to say. Put it here and not in the Canvas comments!
