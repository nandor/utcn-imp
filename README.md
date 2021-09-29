# Imp

*Imp* is a simple, incomplete, interpreted, imperative programming language.
The interpreter bundled here consists of a lexer and parser producing an
abstract syntax tree, following by a simple code generator lowering it to
stack-based bytecode.

### Build

To build the *Imp* interpreter, CMake and a C++17-compatible compiler such
as `gcc` or `clang` is required.
A debug build can be set up using the following commands:


```
mkdir Debug
cd Debug
cmake .. -DCMAKE_BUILD_TYPE=Debug
make
```

### Run

To run the interpreter, provide it with a path to an *Imp* source file:

```
./Debug/imp examples/io.imp
```

### The Imp language

In its current form, *Imp* provides only minimal functionality, as highlighted
by the following example:

```
func print_int(a: int): int = "print_int"
func read_int(): int = "read_int"

func add(a: int, b: int): int {
  return a + b
}

while (read_int()) {
  print_int(add(read_int(), read_int()))
}
```

I/O is performed through external helper methods, provided by the runtime.
To invoke these methods, their prototypes must be declared, naming the function
built into the runtime.

Additional functions can also be defined, naming their arguments and types,
along with the type of a single return value.
The bodies of functions consist of multiple statements.

Instead of a `main` function as an entry point, top-level statements can be
defined anywhere, which are executed in order after the start of the program.

### Project structure

The implementation of the *Imp* interpreter is split across the following files:

- **main.cpp**
Defines the entry point of the interpreter.
Sets up the pipeline, parsing the file, producing bytecode and finally
executing it using the interpreter.

- **lexer.cpp, lexer.h**
Defines the lexical analyser, which splits the stream into a series of tokens.
The tokens correspond to words or symbols from the source file.
Individual tokens also carry information about their location in the sources
to allow accurate diagnostics to be emitted later on.
If unknown tokens are encountered, a `LexerError` is raised.

- **ast.cpp, ast.h**
Defines all the nodes of the Abstract Syntax Tree (AST).
The nodes are organised into a simple hierarchy, all deriving from the
`Node` base class.
Arithmetic expressions are subclasses of `Expr`, while imperative statements
derive from `Stmt`.
At the top level of the script, declarations are grouped into the `TopLevelStmt`
variant.
In order to distinguish subclasses, the base class of a variant defines `Kind`,
which is initialised in the constructor of the appropriate subclass.

- **parser.cpp, parser.h**
The parser consumes the stream of tokens produced by the lexer, constructing the
AST if it is provided with valid syntax and failing with a `ParserError`
otherwise.
The parser is a `LR(1)` parser: recursive-descent with a single look-ahead
token.

- **verifier.cpp, verifier.h**
Currently unused - should implement type checking and other control-flow
integrity checks.

- **codegen.cpp, codegen.h**
Implements the mapping from the AST to bytecode.
The tree is recursively traversed, emitting instructions for all relevant nodes.
The scope chain is also emulated in order to map references to the appropriate
definitions.

- **program.cpp, program.h**
Auxiliary class carrying information about the compiled program, particularly
the stream of bytes representing the compiled bytecode.

- **interp.cpp, interp.h**
Implements the interpreter.
Defines the values which can be stored on the stack and provides a main loop
to decode and evaluate all the bytecode instructions.
The set of bytecode instructions is defined in the `Opcode` enumeration.

- **runtime.cpp, runtime.h**
Implements the runtime support methods invoked by the interpreter, such
as `print_int` and `read_int`.
Runtime methods can inspect and adjust the stack in a manner consistent
with the signature of the prototypes they are defined with.
