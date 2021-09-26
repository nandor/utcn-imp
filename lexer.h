// This file is part of the IMP project.

#pragma once

#include <cassert>
#include <fstream>
#include <iostream>
#include <stdexcept>


/**
 * Represents a location in a source file.
 */
struct Location {
  std::string_view Name;
  int Line;
  int Column;
};

/// Helper to print the location to a stream.
inline std::ostream &operator<<(std::ostream &os, const Location &loc)
{
  os << "[" << loc.Name << ":" << loc.Line << ":" << loc.Column << "]";
  return os;
}

/**
 * Represents a single token of the source language.
 *
 * Tokens are identified through their kind.
 * Certain kinds, such as integers, carry an additional payload.
 */
class Token final {
public:
  enum class Kind {
    // Keywords.
    FUNC,
    RETURN,
    WHILE,
    // Symbols.
    LPAREN,
    RPAREN,
    LBRACE,
    RBRACE,
    COLON,
    SEMI,
    EQUAL,
    COMMA,
    PLUS,
    // Complex tokens.
    INT,
    STRING,
    IDENT,
    END,
  };

public:
  /// Copy constructor.
  Token(const Token &that);
  /// Default constructor, EOF token.
  Token() : kind_(Kind::END) {}
  /// Cleanup.
  ~Token();

  /// Returns the kind of the token.
  Kind GetKind() const { return kind_; }
  /// Returns the location of the token.
  Location GetLocation() const { return loc_; }

  /// Check whether the token is of a specific kind.
  bool Is(Kind kind) const { return GetKind() == kind; }

  /// Token is true if it is not EOF.
  operator bool() const { return GetKind() != Kind::END; }

  /// Return the identifier.
  std::string_view GetIdent() const
  {
    assert(Is(Kind::IDENT) && "not an identifier");
    return *value_.StringValue;
  }

  /// Return the string value.
  std::string_view GetString() const
  {
    assert(Is(Kind::STRING) && "not an identifier");
    return *value_.StringValue;
  }

  /// Copy operator.
  Token &operator=(const Token &that);

  // Helpers to build tokens.
  static Token End(const Location &l) { return Token(l, Kind::END); }
  static Token LParen(const Location &l) { return Token(l, Kind::LPAREN); }
  static Token RParen(const Location &l) { return Token(l, Kind::RPAREN); }
  static Token LBrace(const Location &l) { return Token(l, Kind::LBRACE); }
  static Token RBrace(const Location &l) { return Token(l, Kind::RBRACE); }
  static Token Colon(const Location &l) { return Token(l, Kind::COLON); }
  static Token Semi(const Location &l) { return Token(l, Kind::SEMI); }
  static Token Equal(const Location &l) { return Token(l, Kind::EQUAL); }
  static Token Plus(const Location &l) { return Token(l, Kind::PLUS); }
  static Token Comma(const Location &l) { return Token(l, Kind::COMMA); }
  static Token Func(const Location &l) { return Token(l, Kind::FUNC); }
  static Token Return(const Location &l) { return Token(l, Kind::RETURN); }
  static Token While(const Location &l) { return Token(l, Kind::WHILE); }
  static Token Ident(const Location &l, const std::string &str);
  static Token String(const Location &l, const std::string &str);

  /// Print the token to a stream.
  void Print(std::ostream &os) const;

private:
  /// Create a new token of a given kind.
  Token(const Location &loc, Kind kind) : loc_(loc), kind_(kind) {}

private:
  /// Location of the token.
  Location loc_;
  /// Kind of the token.
  Kind kind_;

  /// Union of all payloads.
  union {
    uint64_t IntValue;
    std::string *StringValue;
  } value_;
};

/// Helper to print a token kind to a stream.
std::ostream &operator<<(std::ostream &os, const Token::Kind kind);
/// Helper to print a token to a stream.
inline std::ostream &operator<<(std::ostream &os, const Token &tk)
{
  tk.Print(os);
  return os;
}

/**
 * Represents a lexer error.
 */
class LexerError : public std::runtime_error {
public:
  LexerError(const Location &loc, const std::string &msg);
};

/**
 * Splits a stream of characters into a stream of tokens.
 */
class Lexer final {
public:
  /// Initialise the lexer, reading the file located at 'name'.
  Lexer(const std::string &name);

  /// Advance the stream to the next token.
  const Token &Next();
  /// Return the current token.
  const Token &GetToken() const { return tk_; }

private:
  /// Advance the stream to the next character. Return '\0' on EOF.
  void NextChar();
  /// Return the location of the current token.
  Location GetLocation() const { return { name_, lineNo_, charNo_ }; }
  /// Report an error.
  [[noreturn]] void Error(const std::string &msg);

private:
  /// Current file name.
  const std::string name_;
  /// Current line number.
  int lineNo_ = 1;
  /// Current character number.
  int charNo_ = 1;
  /// Current character.
  char chr_ = '\0';
  /// Current stream.
  std::ifstream is_;
  /// Current token.
  Token tk_;
};
