# SyntaxTrees

PA3 - Syntax Trees

TODO

## Compile and run program

1. Compile:

```bash
make
```

2. Run:

| Build AST + write to file                         | Evaluate AST + print ouput    |
| ------------------------------------------------- | ----------------------------- |
| `./program <ast-out-file> <expr-in-file>`         | `./program --eval <ast-file>` |
| `./program <ast-out-file> (read expr from stdin)` |                               |

<expr-in-file> : The file from which the expression will be read. If the file does not exist, an error will occur.
<as-out-file> : The file to which the AST will be written. If the file already exists, it will be overwritten.

- When building AST: you can choose to read the expression from a file or from standard input. If you choose to read from standard input, you can type the expression directly into the terminal and press Enter.
- When evaluating AST: you need to provide the AST file that was generated in the previous step. The program will read the AST from the file, evaluate it, and print the output to the terminal.

### Clean

Remove .o files and the executable:

```bash
make clean
```

### What needs to be written in the terminal to compile and run the program, explaining how to run each of the two parts.

### What additional features were implemented above the base version, if any.

### An explanation of the format you store the AST (Abstract Syntax Tree) in that the two versions read/write.

### Anything else a student wishes to say. Put it here and not in the Canvas comments!
