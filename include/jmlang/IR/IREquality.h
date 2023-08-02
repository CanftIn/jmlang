#ifndef JMLANG_IR_IR_EQUALITY_H
#define JMLANG_IR_IR_EQUALITY_H

#include "jmlang/IR/IR.h"

namespace jmlang {
namespace internal {

/** Compare IR nodes for equality of value. Traverses entire IR
 * tree. For equality of reference, use Expr::same_as */
bool equal(Expr a, Expr b);
bool equal(Stmt a, Stmt b);

/** Computes a lexical ordering on IR nodes. Returns -1 if the first
 * expression is before the second, 0 if they're equal, and 1 if the
 * first expression is after the second. */
int deep_compare(Expr a, Expr b);
int deep_compare(Stmt a, Stmt b);

/** A compare struct suitable for use in std::map and std::set that
 * uses the ordering defined by deep_compare. */
struct ExprDeepCompare {
  bool operator()(const Expr& a, const Expr& b) {
    return deep_compare(a, b) < 0;
  }
};

/** A compare struct suitable for use in std::map and std::set that
 * uses the ordering defined by deep_compare. */
struct StmtDeepCompare {
  bool operator()(const Stmt& a, const Stmt& b) {
    return deep_compare(a, b) < 0;
  }
};

}  // namespace internal
}  // namespace jmlang

#endif  // JMLANG_IR_IR_EQUALITY_H