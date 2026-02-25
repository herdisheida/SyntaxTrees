# SyntaxTrees

PA3 - Syntax Trees

## Compile and run program

1. Compile:

```bash
make
```

2. Run:

- **When building AST**: you can choose to read the expression from a file or from standard input. If you choose to read from standard input, you can type the expression directly into the terminal and press Enter.
- **When evaluating AST**: you need to provide the AST file that was generated in the previous step. The program will read the AST from the file, evaluate it, and print the output to the terminal.

| Build AST                                         | Evaluate AST                             |
| ------------------------------------------------- | ---------------------------------------- |
| `./program <ast-out-file> <expr-in-file>`         | `./program --eval <ast-file>`            |
| `./program <ast-out-file> (read expr from stdin)` | `./program --eval <ast-file> <var-file>` |

- **&lt;ast-out-file&gt;** : Output file - AST string representation. If the file already exists, it will be overwritten.
- **&lt;expr-in-file&gt;** : Input file - read expression string. If the file does not exist, an error will occur.
- **&lt;var-file&gt;** : Input file - read variable values. If the file does not exist, an error will occur. If no &lt;var-file&gt; is provided, it will assume there are no variables in the expression.

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
