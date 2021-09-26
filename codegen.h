// This file is part of the IMP project.

#pragma once

#include <memory>
#include <optional>
#include <unordered_map>

#include "program.h"
#include "ast.h"
#include "runtime.h"



/**
 * Translator from the AST to bytecode.
 */
class Codegen {
public:
  /// Entry point to the code generator: translated an entire module.
  std::unique_ptr<Program> Translate(const Module &mod);

private:
  /// Descriptor for a label.
  struct Label {
    explicit Label(unsigned id) : ID(id) {}
    bool operator==(const Label &that) const { return ID == that.ID; }
    unsigned ID;
  };

  /// Helper hash function for the label.
  struct LabelHash {
    size_t operator() (const Label &l) const { return l.ID; }
  };

  /// Specifies the location and kind of the object a name is bound to.
  struct Binding {
    enum class Kind {
      FUNC,
      PROTO,
      ARG,
    } Kind;

    union {
      uint32_t Index;
      RuntimeFn Fn;
      Label Entry;
    };

    Binding() {}
  };

  /// Generic link in the scope chain, mapping identifiers to locations.
  class Scope {
  public:
    Scope(const Scope *parent) : parent_(parent) {}

    virtual ~Scope();

    virtual Binding Lookup(const std::string &name) const = 0;

  protected:
    const Scope *parent_;
  };

  /// Scope for top-level globals.
  class GlobalScope final : public Scope {
  public:
    GlobalScope(
        const std::map<std::string, Label> &funcs,
        const std::map<std::string, RuntimeFn> &protos)
      : Scope(nullptr)
      , funcs_(std::move(funcs))
      , protos_(std::move(protos))
    {
    }

    Binding Lookup(const std::string &name) const override;

  private:
    const std::map<std::string, Label> &funcs_;
    const std::map<std::string, RuntimeFn> &protos_;
  };

  /// Scope for the arguments of a function.
  class FuncScope final : public Scope {
  public:
    FuncScope(
        const Scope *parent,
        const std::map<std::string, uint32_t> &args)
      : Scope(parent)
      , args_(args)
    {
    }

    Binding Lookup(const std::string &name) const override;

  private:
    const std::map<std::string, uint32_t> &args_;
  };

  /// Scope for a block of statements.
  class BlockScope final : public Scope {
  public:
    BlockScope(const Scope *parent) : Scope(parent) {}

    Binding Lookup(const std::string &name) const override;
  };

private:
  /// Lowers a single statement.
  void LowerStmt(const Scope &scope, const Stmt &stmt);
  /// Lowers a block statement.
  void LowerBlockStmt(const Scope &scope, const BlockStmt &blockStmt);
  /// Lowers a while statement.
  void LowerWhileStmt(const Scope &scope, const WhileStmt &whileStmt);
  /// Lowers a return statement.
  void LowerReturnStmt(const Scope &scope, const ReturnStmt &returnStmt);
  /// Lowers a standalone expression statement.
  void LowerExprStmt(const Scope &scope, const ExprStmt &exprStmt);

  /// Lowers a single expression.
  void LowerExpr(const Scope &scope, const Expr &expr);
  /// Lowers a reference to an identifier.
  void LowerRefExpr(const Scope &scope, const RefExpr &expr);
  /// Lowers a binary expression.
  void LowerBinaryExpr(const Scope &scope, const BinaryExpr &expr);
  /// Lowers a call expression.
  void LowerCallExpr(const Scope &scope, const CallExpr &expr);

  /// Lowers a function declaration.
  void LowerFuncDecl(const Scope &scope, const FuncDecl &funcDecl);

private:
  /// Create a new label.
  Label MakeLabel();

  /// Emit a pop instruction.
  void EmitPop();
  /// Emit a call instruction.
  void EmitCall(unsigned nargs);
  /// Push a function address to the stack.
  void EmitPushFunc(Label entry);
  /// Push a prototype to the stack.
  void EmitPushProto(RuntimeFn fn);
  /// Push the nth value from the stack to the top.
  void EmitPeek(uint32_t index);
  /// Emit a return instruction.
  void EmitReturn();
  /// Emit an add opcode.
  void EmitAdd();
  /// Emit a label.
  void EmitLabel(Label label);
  /// Emit a conditional jump.
  void EmitJumpFalse(Label label);
  /// Emit an unconditional jump.
  void EmitJump(Label label);

  /// Emit some bytes of code.
  template<typename T>
  void Emit(const T &t);
  /// Emit an address or create a fixup for later.
  void EmitFixup(Label label);

private:
  /// Reference to the program constructed by the code generator.
  std::vector<uint8_t> code_;
  /// Current stack depth.
  unsigned depth_ = 0;
  /// Current function being compiled.
  const FuncDecl *func_;
  /// Identifier of the next label.
  unsigned nextLabel_ = 0;

  /**
   * A fixup keeps track of all the forward references which must
   * be re-written once the location of a symbol is resolved.
   */
  std::unordered_map<Label, std::vector<size_t>, LabelHash> fixups_;
  /// Mapping from labels to their addresses.
  std::unordered_map<Label, unsigned, LabelHash> labelToAddress_;
  /// Mapping from functions to their entry labels.
  std::map<std::string, Label> funcs_;
};
