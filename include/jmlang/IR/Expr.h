#ifndef JMLANG_IR_EXPR_H
#define JMLANG_IR_EXPR_H

#include "jmlang/IR/IRHandle.h"

namespace jmlang {

/// It's implemented as reference-counted handle to a
/// concrete expression node, but it's immutable, so you
/// can treat it as a value type.
struct Expr : public internal::IRHandle {
  /// Make an undefined expression.
  Expr() {}

  /// Make an expression from a concrete expression node pointer.
  Expr(const internal::BaseExprNode* p) : internal::IRHandle(p) {}

  /// Make an expression representing a const 32-bit int.
  Expr(int x) : internal::IRHandle(internal::IntImm::make(x)) {}

  /// Make an expression representing a const 32-bit float.
  Expr(float x) : internal::IRHandle(internal::FloatImm::make(x)) {}

  /// Make an expression representing a const string
  Expr(const std::string& x)
      : internal::IRHandle(internal::StringImm::make(x)) {}

  /// Get the type of this expression node
  Type type() const { return ((internal::BaseExprNode*)ptr)->type; }
};

struct ExprCompare {
  bool operator()(Expr a, Expr b) const { return a.ptr < b.ptr; }
};

}  // namespace jmlang

#endif  // JMLANG_IR_EXPR_H