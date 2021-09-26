// This file is part of the IMP project.

#pragma once

#include <vector>
#include <memory>
#include <variant>


/**
 * Base class for all AST nodes.
 */
class Node {
};

/**
 * Base class for all statements.
 */
class Stmt : public Node {
public:
  enum class Kind {
    BLOCK,
    WHILE,
    EXPR,
    RETURN
  };

public:
  Kind GetKind() const { return kind_; }

protected:
  Stmt(Kind kind) : kind_(kind) { }

private:
  /// Kind of the statement.
  Kind kind_;
};

/**
 * Base class for all expressions.
 */
class Expr : public Node {
public:
  enum class Kind {
    REF,
    BINARY,
    CALL,
  };

public:
  Expr(Kind kind) : kind_(kind) {}

  Kind GetKind() const { return kind_; }

private:
  /// Kind of the expression.
  Kind kind_;
};

/**
 * Expression referring to a named value.
 */
class RefExpr : public Expr {
public:
  RefExpr(const std::string &name)
    : Expr(Kind::REF)
    , name_(name)
  {
  }

  const std::string &GetName() const { return name_; }

private:
  /// Name of the identifier.
  std::string name_;
};

/**
 * Binary expression.
 */
class BinaryExpr : public Expr {
public:
  /// Enumeration of binary operators.
  enum class Kind {
    ADD
  };

public:
  BinaryExpr(Kind kind, std::shared_ptr<Expr> lhs, std::shared_ptr<Expr> rhs)
    : Expr(Expr::Kind::BINARY)
    , kind_(kind), lhs_(lhs), rhs_(rhs)
  {
  }

  Kind GetKind() const { return kind_; }

  const Expr &GetLHS() const { return *lhs_; }
  const Expr &GetRHS() const { return *rhs_; }

private:
  /// Operator kind.
  Kind kind_;
  /// Left-hand operand.
  std::shared_ptr<Expr> lhs_;
  /// Right-hand operand.
  std::shared_ptr<Expr> rhs_;
};

/**
 * Call expression.
 */
class CallExpr : public Expr {
public:
  using ArgList = std::vector<std::shared_ptr<Expr>>;

public:
  CallExpr(
      std::shared_ptr<Expr> callee,
      std::vector<std::shared_ptr<Expr>> &&args)
    : Expr(Kind::CALL)
    , callee_(callee)
    , args_(std::move(args))
  {
  }

  const Expr &GetCallee() const { return *callee_; }

  size_t arg_size() const { return args_.size(); }
  ArgList::const_reverse_iterator arg_rbegin() const { return args_.rbegin(); }
  ArgList::const_reverse_iterator arg_rend() const { return args_.rend(); }

private:
  std::shared_ptr<Expr> callee_;
  ArgList args_;
};

/**
 * Block statement composed of a sequence of statements.
 */
class BlockStmt final : public Stmt {
public:
  using BlockList = std::vector<std::shared_ptr<Stmt>>;

public:
  BlockStmt(std::vector<std::shared_ptr<Stmt>> &&body)
    : Stmt(Kind::BLOCK)
    , body_(body)
  {
  }

  BlockList::const_iterator begin() const { return body_.begin(); }
  BlockList::const_iterator end() const { return body_.end(); }

private:
  /// Statements in the body of the block.
  BlockList body_;
};

/**
 * Top-level expression statement.
 */
class ExprStmt final : public Stmt {
public:
  ExprStmt(std::shared_ptr<Expr> expr)
    : Stmt(Kind::EXPR)
    , expr_(expr)
  {
  }

  const Expr &GetExpr() const { return *expr_; }

private:
  /// Top-level expression.
  std::shared_ptr<Expr> expr_;
};

/**
 * Return statement returning an expression.
 */
class ReturnStmt final : public Stmt {
public:
  ReturnStmt(std::shared_ptr<Expr> expr)
    : Stmt(Kind::RETURN)
    , expr_(expr)
  {
  }

  const Expr &GetExpr() const { return *expr_; }

private:
  /// Expression to be returned.
  std::shared_ptr<Expr> expr_;
};

/**
 * While statement.
 *
 * while (<cond>) <stmt>
 */
class WhileStmt final : public Stmt {
public:
  WhileStmt(std::shared_ptr<Expr> cond, std::shared_ptr<Stmt> stmt)
    : Stmt(Kind::WHILE)
    , cond_(cond)
    , stmt_(stmt)
  {
  }

  const Expr &GetCond() const { return *cond_; }
  const Stmt &GetStmt() const { return *stmt_; }

private:
  /// Condition for the loop.
  std::shared_ptr<Expr> cond_;
  /// Expression to be executed in the loop body.
  std::shared_ptr<Stmt> stmt_;
};

/**
 * Base class for internal and external function declarations.
 */
class FuncOrProtoDecl : public Node {
public:
  using ArgList = std::vector<std::pair<std::string, std::string>>;

public:
  FuncOrProtoDecl(
      const std::string &name,
      std::vector<std::pair<std::string, std::string>> &&args,
      const std::string &type)
    : name_(name)
    , args_(std::move(args))
    , type_(type)
  {
  }

  virtual ~FuncOrProtoDecl();

  const std::string &GetName() const { return name_; }

  size_t arg_size() const { return args_.size(); }
  ArgList::const_iterator arg_begin() const { return args_.begin(); }
  ArgList::const_iterator arg_end() const { return args_.end(); }

private:
  /// Name of the declaration.
  const std::string name_;
  /// Argument list.
  ArgList args_;
  /// Return type identifier.
  const std::string &type_;
};

/**
 * External function prototype declaration.
 *
 * func proto(a: int): int = "proto"
 */
class ProtoDecl final : public FuncOrProtoDecl {
public:
  ProtoDecl(
      const std::string &name,
      std::vector<std::pair<std::string, std::string>> &&args,
      const std::string &type,
      const std::string &primitive)
    : FuncOrProtoDecl(name, std::move(args), type)
    , primitive_(primitive)
  {
  }

  const std::string &GetPrimitiveName() const { return primitive_; }

private:
  const std::string primitive_;
};

/**
 * Function declaration.
 *
 * func test(a: int): int = { ... }
 */
class FuncDecl final : public FuncOrProtoDecl {
public:
  FuncDecl(
      const std::string &name,
      std::vector<std::pair<std::string, std::string>> &&args,
      const std::string &type,
      std::shared_ptr<BlockStmt> body)
    : FuncOrProtoDecl(name, std::move(args), type)
    , body_(body)
  {
  }

  const BlockStmt &GetBody() const { return *body_; }

private:
  std::shared_ptr<BlockStmt> body_;
};

/// Alternative for a toplevel construct.
using TopLevelStmt = std::variant
    < std::shared_ptr<FuncDecl>
    , std::shared_ptr<ProtoDecl>
    , std::shared_ptr<Stmt>
    >;

/**
 * Main node of the AST, capturing information about the program.
 */
class Module final : public Node {
public:
  using BlockList = std::vector<TopLevelStmt>;

public:
  Module(
      std::vector<TopLevelStmt> &&body)
    : body_(body)
  {
  }

  BlockList::const_iterator begin() const { return body_.begin(); }
  BlockList::const_iterator end() const { return body_.end(); }

private:
  BlockList body_;
};
