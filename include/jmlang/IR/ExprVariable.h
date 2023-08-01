#ifndef JMLANG_IR_EXPR_VARIABLE_H
#define JMLANG_IR_EXPR_VARIABLE_H

#include "jmlang/IR/Function.h"
#include "jmlang/IR/Parameter.h"

namespace jmlang {
namespace internal {

/// A named variable. Might be a loop variable, function argument,
/// parameter, reduction variable, or something defined by a Let or
/// LetStmt node.
struct Variable : public ExprNode<Variable> {
  std::string name;

  /// References to scalar parameters, or to the dimensions of buffer
  /// parameters hang onto those expressions.
  Parameter param;

  /// Reduction variables hang onto their domains.
  ReductionDomain reduction_domain;

  static Expr make(Type type, std::string name) {
    return make(type, name, Parameter(), ReductionDomain());
  }

  static Expr make(Type type, std::string name, Parameter param) {
    return make(type, name, param, ReductionDomain());
  }

  static Expr make(Type type, std::string name,
                   ReductionDomain reduction_domain) {
    return make(type, name, Parameter(), reduction_domain);
  }

  static Expr make(Type type, std::string name, Parameter param,
                   ReductionDomain reduction_domain) {
    Variable* node = new Variable;
    node->type = type;
    node->name = name;
    node->param = param;
    node->reduction_domain = reduction_domain;
    return node;
  }
};

}  // namespace internal
}  // namespace jmlang

#endif  // JMLANG_IR_EXPR_VARIABLE_H