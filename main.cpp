// This file is part of the IMP project.

#include <iostream>

#include "ast.h"
#include "codegen.h"
#include "interp.h"
#include "lexer.h"
#include "parser.h"
#include "verifier.h"



// -----------------------------------------------------------------------------
int main(int argc, char **argv)
{
  const char *exeName = argc < 1 ? "imp" : argv[0];

  if (argc != 2) {
    std::cerr << "Usage: " << exeName << " path-to-file" << std::endl;
    return EXIT_FAILURE;
  }

  try {
    // The lexer splits the source into a stream of tokens.
    Lexer lexer(argv[1]);

    // The parser processes the tokens from the lexer to build the AST.
    auto ast = Parser(lexer).ParseModule();

    // The verifier checks the program and emits warnings/errors.
    Verifier().Verify(*ast);

    // The code generator translates the AST into bytecode.
    auto prog = Codegen().Translate(*ast);

    // The bytecode interpreter runs the bytecode.
    Interp(*prog).Run();

  } catch (const std::exception &ex) {
    // Report any exceptions (parser, lexer, verification, runtime errors).
    std::cerr << ex.what() << std::endl;
    return EXIT_FAILURE;
  }

  return EXIT_SUCCESS;
}
