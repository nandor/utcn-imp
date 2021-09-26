// This file is part of the IMP project.

#include <sstream>

#include "lexer.h"



// -----------------------------------------------------------------------------
Token::Token(const Token &that)
  : loc_(that.loc_)
  , kind_(that.kind_)
{
  switch (kind_) {
    case Kind::STRING:
    case Kind::IDENT: {
      value_.StringValue = new std::string(*that.value_.StringValue);
      break;
    }
    default: {
      break;
    }
  }
}

// -----------------------------------------------------------------------------
Token &Token::operator=(const Token &that)
{
  switch (kind_) {
    case Kind::STRING:
    case Kind::IDENT: {
      delete value_.StringValue;
      break;
    }
    default: {
      break;
    }
  }
  loc_ = that.loc_;
  kind_ = that.kind_;
  switch (kind_) {
    case Kind::STRING:
    case Kind::IDENT: {
      value_.StringValue = new std::string(*that.value_.StringValue);
      break;
    }
    default: {
      break;
    }
  }
  return *this;
}

// -----------------------------------------------------------------------------
Token::~Token()
{
  switch (kind_) {
    case Kind::STRING:
    case Kind::IDENT: {
      delete value_.StringValue;
      break;
    }
    default: {
      break;
    }
  }
}

// -----------------------------------------------------------------------------
Token Token::Ident(const Location &l, const std::string &str)
{
  Token tk(l, Kind::IDENT);
  tk.value_.StringValue = new std::string(str);
  return tk;
}

// -----------------------------------------------------------------------------
Token Token::String(const Location &l, const std::string &str)
{
  Token tk(l, Kind::STRING);
  tk.value_.StringValue = new std::string(str);
  return tk;
}

// -----------------------------------------------------------------------------
void Token::Print(std::ostream &os) const
{
  os << kind_;
  switch (kind_) {
    case Kind::INT: {
      os << "(" << value_.IntValue << ")";
      break;
    }
    case Kind::STRING: {
      os << "(\"" << *value_.StringValue << "\")";
      break;
    }
    case Kind::IDENT: {
      os << "(" << *value_.StringValue << ")";
      break;
    }
    default: {
      break;
    }
  }
}

// -----------------------------------------------------------------------------
std::ostream &operator<<(std::ostream &os, const Token::Kind kind)
{
  switch (kind) {
    case Token::Kind::FUNC: return os << "func";
    case Token::Kind::RETURN: return os << "return";
    case Token::Kind::WHILE: return os << "while";
    case Token::Kind::LPAREN: return os << "(";
    case Token::Kind::RPAREN: return os << ")";
    case Token::Kind::LBRACE: return os << "{";
    case Token::Kind::RBRACE: return os << "}";
    case Token::Kind::COLON: return os << ":";
    case Token::Kind::SEMI: return os << ";";
    case Token::Kind::EQUAL: return os << "=";
    case Token::Kind::COMMA: return os << ",";
    case Token::Kind::PLUS: return os << "+";
    case Token::Kind::END: return os << "END";
    case Token::Kind::INT: return os << "INT";
    case Token::Kind::STRING: return os << "STRING";
    case Token::Kind::IDENT: return os << "IDENT";
  }
  return os;
}

// -----------------------------------------------------------------------------
static std::string FormatMessage(const Location &loc, const std::string &msg)
{
  std::ostringstream os;
  os << "[" << loc.Name << ":" << loc.Line << ":" << loc.Column << "] " << msg;
  return os.str();
}

// -----------------------------------------------------------------------------
LexerError::LexerError(const Location &loc, const std::string &msg)
  : std::runtime_error(FormatMessage(loc, msg))
{
}

// -----------------------------------------------------------------------------
Lexer::Lexer(const std::string &name)
  : name_(name)
  , is_(name)
{
  NextChar();
  Next();
}

// -----------------------------------------------------------------------------
static bool IsIdentStart(char chr)
{
  return chr == '_' || isalpha(chr);
}

// -----------------------------------------------------------------------------
static bool IsIdentLetter(char chr)
{
  return IsIdentStart(chr) || isdigit(chr);
}

// -----------------------------------------------------------------------------
const Token &Lexer::Next()
{
  // Skip all whitespace until a valid token.
  while (isspace(chr_)) { NextChar(); }

  // Return a token based on the character.
  auto loc = GetLocation();
  switch (chr_) {
    case '\0': return tk_ = Token::End(loc);
    case '(': return NextChar(), tk_ = Token::LParen(loc);
    case ')': return NextChar(), tk_ = Token::RParen(loc);
    case '{': return NextChar(), tk_ = Token::LBrace(loc);
    case '}': return NextChar(), tk_ = Token::RBrace(loc);
    case ':': return NextChar(), tk_ = Token::Colon(loc);
    case ';': return NextChar(), tk_ = Token::Semi(loc);
    case '=': return NextChar(), tk_ = Token::Equal(loc);
    case '+': return NextChar(), tk_ = Token::Plus(loc);
    case ',': return NextChar(), tk_ = Token::Comma(loc);
    case '"': {
      std::string word;
      NextChar();
      while (chr_ != '"') {
        word.push_back(chr_);
        NextChar();
        if (chr_ == '\0') {
          Error("string not terminated");
        }
      }
      NextChar();
      return tk_ = Token::String(loc, word);
    }
    default: {
      if (IsIdentStart(chr_)) {
        std::string word;
        do {
          word.push_back(chr_);
          NextChar();
        } while (IsIdentLetter(chr_));
        if (word == "func") return tk_ = Token::Func(loc);
        if (word == "return") return tk_ = Token::Return(loc);
        if (word == "while") return tk_ = Token::While(loc);
        return tk_ = Token::Ident(loc, word);
      }
      Error("unknown character '" + std::string(1, chr_) + "'");
    }
  }
}

// -----------------------------------------------------------------------------
void Lexer::NextChar()
{
  if (is_.eof()) {
    chr_ = '\0';
  } else {
    if (chr_ == '\n') {
      lineNo_++;
      charNo_ = 1;
    } else {
      charNo_++;
    }
    is_.get(chr_);
  }
}

// -----------------------------------------------------------------------------
void Lexer::Error(const std::string &msg)
{
  throw LexerError(GetLocation(), msg);
}
