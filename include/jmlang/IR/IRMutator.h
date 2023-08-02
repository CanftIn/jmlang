#ifndef JMLANG_IR_IR_MUTATOR_H
#define JMLANG_IR_IR_MUTATOR_H

#include <utility>
#include <vector>

#include "jmlang/IR/ExprCall.h"
#include "jmlang/IR/ExprVariable.h"
#include "jmlang/IR/IR.h"
#include "jmlang/IR/IRVisitor.h"

namespace jmlang {
namespace internal {

/// A base class for passes over the IR which modify it
/// (e.g. replacing a variable with a value (Substitute.h), or
/// constant-folding).
class IRMutator : public IRVisitor {
 public:
  virtual Expr mutate(Expr expr);
  virtual Stmt mutate(Stmt stmt);

 protected:
  Expr expr;

  Stmt stmt;

  virtual void visit(const IntImm*);
  virtual void visit(const FloatImm*);
  virtual void visit(const StringImm*);
  virtual void visit(const Cast*);
  virtual void visit(const Variable*);
  virtual void visit(const Add*);
  virtual void visit(const Sub*);
  virtual void visit(const Mul*);
  virtual void visit(const Div*);
  virtual void visit(const Mod*);
  virtual void visit(const Min*);
  virtual void visit(const Max*);
  virtual void visit(const EQ*);
  virtual void visit(const NE*);
  virtual void visit(const LT*);
  virtual void visit(const LE*);
  virtual void visit(const GT*);
  virtual void visit(const GE*);
  virtual void visit(const And*);
  virtual void visit(const Or*);
  virtual void visit(const Not*);
  virtual void visit(const Select*);
  virtual void visit(const Load*);
  virtual void visit(const Ramp*);
  virtual void visit(const Broadcast*);
  virtual void visit(const Call*);
  virtual void visit(const Let*);
  virtual void visit(const LetStmt*);
  virtual void visit(const AssertStmt*);
  virtual void visit(const Pipeline*);
  virtual void visit(const For*);
  virtual void visit(const Store*);
  virtual void visit(const Provide*);
  virtual void visit(const Allocate*);
  virtual void visit(const Free*);
  virtual void visit(const Realize*);
  virtual void visit(const Block*);
  virtual void visit(const IfThenElse*);
  virtual void visit(const Evaluate*);
};

}  // namespace internal
}  // namespace jmlang

#endif  // JMLANG_IR_IR_MUTATOR_H