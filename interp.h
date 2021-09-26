// This file is part of the IMP project.

#pragma once

#include <cassert>
#include <vector>

#include "runtime.h"

class Program;



/**
 * Represents a runtime error.
 */
class RuntimeError : public std::runtime_error {
public:
  RuntimeError(const std::string &msg) : std::runtime_error(msg) {}
};

/**
 * Interpreter for the bytecode.
 */
class Interp {
public:
  /// A dynamically-typed value stored on top of the stack.
  struct Value {
    enum class Kind {
      PROTO,
      ADDR,
      INT,
    } Kind;

    union {
      RuntimeFn Proto;
      size_t Addr;
      int64_t Int;
    } Val;

    Value() : Kind(Kind::INT) { Val.Int = 0; }
    Value(RuntimeFn val) : Kind(Kind::PROTO) { Val.Proto = val; }
    Value(size_t val) : Kind(Kind::ADDR) { Val.Addr = val; }
    Value(int64_t val) : Kind(Kind::INT) { Val.Int = val; }

    operator bool () const
    {
      switch (Kind) {
        case Kind::PROTO: return true;
        case Kind::ADDR: return true;
        case Kind::INT: return Val.Int != 0;
      }
      return false;
    }
  };

public:
  /// Creates an interpreter for a given program.
  Interp(Program &prog) : prog_(prog) {}

  /// Interpreter main loop.
  void Run();

  /// Pop a value from the stack.
  Value Pop()
  {
    assert(!stack_.empty() && "stack empty");
    auto t = *stack_.rbegin();
    stack_.pop_back();
    return t;
  }

  /// Pop an integer from the stack.
  int64_t PopInt()
  {
    auto v = Pop();
    assert(v.Kind == Value::Kind::INT);
    return v.Val.Int;
  }

  /// Pop an address from the stack.
  int64_t PopAddr()
  {
    auto v = Pop();
    assert(v.Kind == Value::Kind::ADDR);
    return v.Val.Addr;
  }

  /// Look at the integer on top of the stack.
  int64_t PeekInt()
  {
    auto v = *stack_.rbegin();
    assert(v.Kind == Value::Kind::INT);
    return v.Val.Int;
  }

  /// Add a value to the stack.
  template <typename T>
  void Push(const T &t)
  {
    stack_.emplace_back(std::forward<const T>(t));
  }

private:
  /// Reference to the program being executed.
  Program &prog_;
  /// Program counter.
  size_t pc_ = 0;
  /// Evaluation stack.
  std::vector<Value> stack_;
};
