# SyntaxTrees

PA3 - Syntax Trees

## Compile and run program

1. Compile:

```bash
make
```

2. Run:

| Build AST + write to file                         | Evaluate AST + print ouput               |
| ------------------------------------------------- | ---------------------------------------- |
| `./program <ast-out-file> <expr-in-file>`         | `./program --eval <ast-file>`            |
| `./program <ast-out-file> (read expr from stdin)` | `./program --eval <ast-file> <var-file>` |

<ast-out-file> : The file to which the AST will be written. If the file already exists, it will be overwritten.
<expr-in-file> : The file from which the expression will be read. If the file does not exist, an error will occur.
<var-file> : The file from which the variable values will be read. If the file does not exist, it will be assumed that there are no variables in the expression. (throw error if there are variables in the expression but no var-file is provided)

- When building AST: you can choose to read the expression from a file or from standard input. If you choose to read from standard input, you can type the expression directly into the terminal and press Enter.
- When evaluating AST: you need to provide the AST file that was generated in the previous step. The program will read the AST from the file, evaluate it, and print the output to the terminal.

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
5. Variables (15%): allow reading variables from file
6. AST optimisations (~10%): while building AST pre-calculate: 0 and 1 multiplication, 0 and 1 division , 0 addition and subtraction

[ ] An explanation of the format you store the AST (Abstract Syntax Tree) in that the two versions read/write.

[ ] Anything else a student wishes to say. Put it here and not in the Canvas comments!
