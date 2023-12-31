#ifndef JMLANG_IR_IRVISITOR_H
#define JMLANG_IR_IRVISITOR_H

#include <set>

namespace jmlang {

struct Expr;

namespace internal {

struct IRNode;
struct Stmt;
struct IntImm;
struct FloatImm;
struct StringImm;
struct Cast;
struct Variable;
struct Add;
struct Sub;
struct Mul;
struct Div;
struct Mod;
struct Min;
struct Max;
struct EQ;
struct NE;
struct LT;
struct LE;
struct GT;
struct GE;
struct And;
struct Or;
struct Not;
struct Select;
struct Load;
struct Ramp;
struct Broadcast;
struct Call;
struct Let;
struct LetStmt;
struct AssertStmt;
struct Pipeline;
struct For;
struct Store;
struct Provide;
struct Allocate;
struct Free;
struct Realize;
struct Block;
struct IfThenElse;
struct Evaluate;

/// A base class for algorithms that need to recursively walk over the
/// IR. The default implementations just recursively walk over the
/// children. Override the ones you care about.
class IRVisitor {
 public:
  virtual ~IRVisitor();
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

/// A base class for algorithms that walk recursively over the IR
/// without visiting the same node twice. This is for passes that are
/// capable of interpreting the IR as a DAG instead of a tree.
class IRGraphVisitor : public IRVisitor {
 protected:
  virtual void include(const Expr&);
  virtual void include(const Stmt&);

  std::set<const IRNode*> visited;

 public:
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

#endif  // JMLANG_IR_IRVISITOR_H