#ifndef JMLANG_IR_IR_PRINTER_H
#define JMLANG_IR_IR_PRINTER_H

#include <ostream>

#include "jmlang/Base/Type.h"
#include "jmlang/IR/ExprCall.h"
#include "jmlang/IR/ExprVariable.h"
#include "jmlang/IR/IR.h"
#include "jmlang/IR/IRVisitor.h"

namespace jmlang {

std::ostream& operator<<(std::ostream& stream, Expr);

std::ostream& operator<<(std::ostream& stream, Type);

namespace internal {

std::ostream& operator<<(std::ostream& stream, Stmt);

std::ostream& operator<<(std::ostream& stream, const For::ForType&);

class IRPrinter : public IRVisitor {
 public:
  IRPrinter(std::ostream&);

  void print(Expr);

  void print(Stmt);

  static void test();

 protected:
  std::ostream& stream;

  int indent;

  void do_indent();

  void visit(const IntImm*);
  void visit(const FloatImm*);
  void visit(const StringImm*);
  void visit(const Cast*);
  void visit(const Variable*);
  void visit(const Add*);
  void visit(const Sub*);
  void visit(const Mul*);
  void visit(const Div*);
  void visit(const Mod*);
  void visit(const Min*);
  void visit(const Max*);
  void visit(const EQ*);
  void visit(const NE*);
  void visit(const LT*);
  void visit(const LE*);
  void visit(const GT*);
  void visit(const GE*);
  void visit(const And*);
  void visit(const Or*);
  void visit(const Not*);
  void visit(const Select*);
  void visit(const Load*);
  void visit(const Ramp*);
  void visit(const Broadcast*);
  void visit(const Call*);
  void visit(const Let*);
  void visit(const LetStmt*);
  void visit(const AssertStmt*);
  void visit(const Pipeline*);
  void visit(const For*);
  void visit(const Store*);
  void visit(const Provide*);
  void visit(const Allocate*);
  void visit(const Free*);
  void visit(const Realize*);
  void visit(const Block*);
  void visit(const IfThenElse*);
  void visit(const Evaluate*);
};

}  // namespace internal

}  // namespace jmlang

#endif  // JMLANG_IR_IR_PRINTER_H