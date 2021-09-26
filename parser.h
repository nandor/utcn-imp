// This file is part of the IMP project.

#pragma once

#include <memory>

#include "lexer.h"
#include "ast.h"



/**
 * Wrapper around the location of a parser error.
 */
class ParserError : public std::runtime_error {
public:
  ParserError(const Location &loc, const std::string &msg);
};

/**
 * Implements the recursive descent parser.
 */
class Parser {
public:
  /// Initialise the parser given a reference to the lexer.
  Parser(Lexer &lexer);

  /**
   * Parse the top-level node, which consists of a series of statements.
   */
  std::shared_ptr<Module> ParseModule();

private:
  /// Parse a single statement.
  std::shared_ptr<Stmt> ParseStmt();
  /// Parse a block of statements.
  std::shared_ptr<BlockStmt> ParseBlockStmt();
  /// Parse a return statement: return <expr>
  std::shared_ptr<ReturnStmt> ParseReturnStmt();
  /// Parse a while loop.
  std::shared_ptr<WhileStmt> ParseWhileStmt();

  /// Parse a single expression.
  std::shared_ptr<Expr> ParseExpr() { return ParseAddSubExpr(); }
  /// Parse an expression which has no operators.
  std::shared_ptr<Expr> ParseTermExpr();
  /// Parse a call expression.
  std::shared_ptr<Expr> ParseCallExpr();
  /// Parse an add/sub expression.
  std::shared_ptr<Expr> ParseAddSubExpr();

  /// Helper to get the current token.
  inline const Token &Current() { return lexer_.GetToken(); }
  /// Check whether the next token is of a given kind, failing otherwise.
  const Token &Expect(Token::Kind kind);
  /// Check whether the current token is of a given kind, failing otherwise.
  const Token &Check(Token::Kind kind);
  /// Report an error.
  [[noreturn]] void Error(const Location &loc, const std::string &msg);

private:
  Lexer &lexer_;
};
