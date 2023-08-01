#ifndef JMLANG_IR_IR_H
#define JMLANG_IR_IR_H

#include "jmlang/Base/Buffer.h"
#include "jmlang/IR/Expr.h"
#include "jmlang/IR/IRHandle.h"
#include "jmlang/IR/Parameter.h"

namespace jmlang {
namespace internal {

/// A reference-counted handle to a statement node.
struct Stmt : public IRHandle {
  Stmt() : IRHandle() {}
  Stmt(const BaseStmtNode* n) : IRHandle(n) {}

  /// This lets you use a Stmt as a key in a map of the form
  /// map<Stmt, Foo, Stmt::Compare>
  struct Compare {
    bool operator()(const Stmt& a, const Stmt& b) const {
      return a.ptr < b.ptr;
    }
  };
};

/// Cast a node from one type to another.
struct Cast : public ExprNode<Cast> {
  Expr value;

  static Expr make(Type t, Expr v) {
    assert(v.defined() && "Cast of undefined");

    Cast* node = new Cast;
    node->type = t;
    node->value = v;
    return node;
  }
};

/// The sum of two expressions.
struct Add : public ExprNode<Add> {
  Expr a, b;

  static Expr make(Expr a, Expr b) {
    assert(a.defined() && "Add of undefined");
    assert(b.defined() && "Add of undefined");
    assert(a.type() == b.type() && "Add of mismatched types");

    Add* node = new Add;
    node->type = a.type();
    node->a = a;
    node->b = b;
    return node;
  }
};

/// The difference of two expressions.
struct Sub : public ExprNode<Sub> {
  Expr a, b;

  static Expr make(Expr a, Expr b) {
    assert(a.defined() && "Sub of undefined");
    assert(b.defined() && "Sub of undefined");
    assert(a.type() == b.type() && "Sub of mismatched types");

    Sub* node = new Sub;
    node->type = a.type();
    node->a = a;
    node->b = b;
    return node;
  }
};

/// The product of two expressions.
struct Mul : public ExprNode<Mul> {
  Expr a, b;

  static Expr make(Expr a, Expr b) {
    assert(a.defined() && "Mul of undefined");
    assert(b.defined() && "Mul of undefined");
    assert(a.type() == b.type() && "Mul of mismatched types");

    Mul* node = new Mul;
    node->type = a.type();
    node->a = a;
    node->b = b;
    return node;
  }
};

/// The ratio of two expressions.
struct Div : public ExprNode<Div> {
  Expr a, b;

  static Expr make(Expr a, Expr b) {
    assert(a.defined() && "Div of undefined");
    assert(b.defined() && "Div of undefined");
    assert(a.type() == b.type() && "Div of mismatched types");

    Div* node = new Div;
    node->type = a.type();
    node->a = a;
    node->b = b;
    return node;
  }
};

/// The remainder of a / b.
struct Mod : public ExprNode<Mod> {
  Expr a, b;

  static Expr make(Expr a, Expr b) {
    assert(a.defined() && "Mod of undefined");
    assert(b.defined() && "Mod of undefined");
    assert(a.type() == b.type() && "Mod of mismatched types");

    Mod* node = new Mod;
    node->type = a.type();
    node->a = a;
    node->b = b;
    return node;
  }
};

/// The lesser of two values.
struct Min : public ExprNode<Min> {
  Expr a, b;

  static Expr make(Expr a, Expr b) {
    assert(a.defined() && "Min of undefined");
    assert(b.defined() && "Min of undefined");
    assert(a.type() == b.type() && "Min of mismatched types");

    Min* node = new Min;
    node->type = a.type();
    node->a = a;
    node->b = b;
    return node;
  }
};

/// The greater of two values.
struct Max : public ExprNode<Max> {
  Expr a, b;

  static Expr make(Expr a, Expr b) {
    assert(a.defined() && "Max of undefined");
    assert(b.defined() && "Max of undefined");
    assert(a.type() == b.type() && "Max of mismatched types");

    Max* node = new Max;
    node->type = a.type();
    node->a = a;
    node->b = b;
    return node;
  }
};

/// Is the first expression equal to the second.
struct EQ : public ExprNode<EQ> {
  Expr a, b;

  static Expr make(Expr a, Expr b) {
    assert(a.defined() && "EQ of undefined");
    assert(b.defined() && "EQ of undefined");
    assert(a.type() == b.type() && "EQ of mismatched types");

    EQ* node = new EQ;
    node->type = Bool(a.type().width);
    node->a = a;
    node->b = b;
    return node;
  }
};

/// Is the first expression not equal to the second.
struct NE : public ExprNode<NE> {
  Expr a, b;

  static Expr make(Expr a, Expr b) {
    assert(a.defined() && "NE of undefined");
    assert(b.defined() && "NE of undefined");
    assert(a.type() == b.type() && "NE of mismatched types");

    NE* node = new NE;
    node->type = Bool(a.type().width);
    node->a = a;
    node->b = b;
    return node;
  }
};

/// Is the first expression less than the second.
struct LT : public ExprNode<LT> {
  Expr a, b;

  static Expr make(Expr a, Expr b) {
    assert(a.defined() && "LT of undefined");
    assert(b.defined() && "LT of undefined");
    assert(a.type() == b.type() && "LT of mismatched types");

    LT* node = new LT;
    node->type = Bool(a.type().width);
    node->a = a;
    node->b = b;
    return node;
  }
};

/// Is the first expression less than or equal to the second.
struct LE : public ExprNode<LE> {
  Expr a, b;

  static Expr make(Expr a, Expr b) {
    assert(a.defined() && "LE of undefined");
    assert(b.defined() && "LE of undefined");
    assert(a.type() == b.type() && "LE of mismatched types");

    LE* node = new LE;
    node->type = Bool(a.type().width);
    node->a = a;
    node->b = b;
    return node;
  }
};

/// Is the first expression greater than the second.
struct GT : public ExprNode<GT> {
  Expr a, b;

  static Expr make(Expr a, Expr b) {
    assert(a.defined() && "GT of undefined");
    assert(b.defined() && "GT of undefined");
    assert(a.type() == b.type() && "GT of mismatched types");

    GT* node = new GT;
    node->type = Bool(a.type().width);
    node->a = a;
    node->b = b;
    return node;
  }
};

/// Is the first expression greater than or equal to the second.
struct GE : public ExprNode<GE> {
  Expr a, b;

  static Expr make(Expr a, Expr b) {
    assert(a.defined() && "GE of undefined");
    assert(b.defined() && "GE of undefined");
    assert(a.type() == b.type() && "GE of mismatched types");

    GE* node = new GE;
    node->type = Bool(a.type().width);
    node->a = a;
    node->b = b;
    return node;
  }
};

/// Logical and : are both expressions true.
struct And : public ExprNode<And> {
  Expr a, b;

  static Expr make(Expr a, Expr b) {
    assert(a.defined() && "And of undefined");
    assert(b.defined() && "And of undefined");
    assert(a.type().is_bool() && "lhs of And is not a bool");
    assert(b.type().is_bool() && "rhs of And is not a bool");

    And* node = new And;
    node->type = Bool(a.type().width);
    node->a = a;
    node->b = b;
    return node;
  }
};

/// Logical or : is at least one of the expression true.
struct Or : public ExprNode<Or> {
  Expr a, b;

  static Expr make(Expr a, Expr b) {
    assert(a.defined() && "Or of undefined");
    assert(b.defined() && "Or of undefined");
    assert(a.type().is_bool() && "lhs of Or is not a bool");
    assert(b.type().is_bool() && "rhs of Or is not a bool");

    Or* node = new Or;
    node->type = Bool(a.type().width);
    node->a = a;
    node->b = b;
    return node;
  }
};

/// Logical not : true if the expression false.
struct Not : public ExprNode<Not> {
  Expr a;

  static Expr make(Expr a) {
    assert(a.defined() && "Not of undefined");
    assert(a.type().is_bool() && "argument of Not is not a bool");

    Not* node = new Not;
    node->type = Bool(a.type().width);
    node->a = a;
    return node;
  }
};

/// A ternary operator. Evalutes 'true_value' and 'false_value',
/// then selects between them based on 'condition'.
struct Select : public ExprNode<Select> {
  Expr condition, true_value, false_value;

  static Expr make(Expr condition, Expr true_value, Expr false_value) {
    assert(condition.defined() && "Select of undefined");
    assert(true_value.defined() && "Select of undefined");
    assert(false_value.defined() && "Select of undefined");
    assert(condition.type().is_bool() &&
           "First argument to Select is not a bool");
    assert(false_value.type() == true_value.type() &&
           "Select of mismatched types");
    assert((condition.type().is_scalar() ||
            condition.type().width == true_value.type().width) &&
           "In Select, vector width of condition must either be 1, or equal to "
           "vector width of arguments");

    Select* node = new Select;
    node->type = true_value.type();
    node->condition = condition;
    node->true_value = true_value;
    node->false_value = false_value;
    return node;
  }
};

/// Load a value from a named buffer. The buffer is treated as an
/// array of the 'type' of this Load node.
struct Load : public ExprNode<Load> {
  std::string name;
  Expr index;

  // If it's a load from an image argument or compiled-in constant
  // image, this will point to that
  Buffer image;

  // If it's a load from an image parameter, this points to that
  Parameter param;

  static Expr make(Type type, std::string name, Expr index, Buffer image,
                   Parameter param) {
    assert(index.defined() && "Load of undefined");
    assert(type.width == index.type().width &&
           "Vector width of Load must match vector width of index");

    Load* node = new Load;
    node->type = type;
    node->name = name;
    node->index = index;
    node->image = image;
    node->param = param;
    return node;
  }
};

/// A linear ramp vector node. This is vector with 'width' elements,
/// where element i is 'base' + i*'stride'. This is a convenient way to
/// pass around vectors without busting them up into individual
/// elements. E.g. a dense vector load from a buffer can use a ramp
/// node with stride 1 as the index.
struct Ramp : public ExprNode<Ramp> {
  Expr base, stride;
  int width;

  static Expr make(Expr base, Expr stride, int width) {
    assert(base.defined() && "Ramp of undefined");
    assert(stride.defined() && "Ramp of undefined");
    assert(base.type().is_scalar() && "Ramp with vector base");
    assert(stride.type().is_scalar() && "Ramp with vector stride");
    assert(width > 1 && "Ramp of width <= 1");
    assert(stride.type() == base.type() && "Ramp of mismatched types");

    Ramp* node = new Ramp;
    node->type = base.type().vector_of(width);
    node->base = base;
    node->stride = stride;
    node->width = width;
    return node;
  }
};

/// A vector with 'width' elements, in which every element is
/// 'value'. This is a special case of the ramp node above, in which
/// the stride is zero.
struct Broadcast : public ExprNode<Broadcast> {
  Expr value;
  int width;

  static Expr make(Expr value, int width) {
    assert(value.defined() && "Broadcast of undefined");
    assert(value.type().is_scalar() && "Broadcast of vector");
    assert(width > 1 && "Broadcast of width <= 1");

    Broadcast* node = new Broadcast;
    node->type = value.type().vector_of(width);
    node->value = value;
    node->width = width;
    return node;
  }
};

/// A let expression, like you might find in a functional
/// language.
struct Let : public ExprNode<Let> {
  std::string name;
  Expr value, body;

  static Expr make(std::string name, Expr value, Expr body) {
    assert(value.defined() && "Let of undefined");
    assert(body.defined() && "Let of undefined");

    Let* node = new Let;
    node->type = body.type();
    node->name = name;
    node->value = value;
    node->body = body;
    return node;
  }
};

/// The statement form of a let node. Within the statement 'body',
/// instances of the Var named 'name' refer to 'value'
struct LetStmt : public StmtNode<LetStmt> {
  std::string name;
  Expr value;
  Stmt body;

  static Stmt make(std::string name, Expr value, Stmt body) {
    assert(value.defined() && "Let of undefined");
    assert(body.defined() && "Let of undefined");

    LetStmt* node = new LetStmt;
    node->name = name;
    node->value = value;
    node->body = body;
    return node;
  }
};

/// If the 'condition' is false, then bail out printing the
/// 'message' to stderr
struct AssertStmt : public StmtNode<AssertStmt> {
  // if condition then val else error out with message
  Expr condition;
  std::string message;

  static Stmt make(Expr condition, std::string message) {
    assert(condition.defined() && "AssertStmt of undefined");

    AssertStmt* node = new AssertStmt;
    node->condition = condition;
    node->message = message;
    return node;
  }
};

/// This node is a helpful annotation to do with permissions. The
/// three child statements happen in order. In the 'produce'
/// statement 'buffer' is write-only. In 'update' it is
/// read-write. In 'consume' it is read-only. The 'update' node is
/// often NULL. (check update.defined() to find out). None of this
/// is actually enforced, the node is purely for informative
/// purposes to help out our analysis during lowering.
struct Pipeline : public StmtNode<Pipeline> {
  std::string name;
  Stmt produce, update, consume;

  static Stmt make(std::string name, Stmt produce, Stmt update, Stmt consume) {
    assert(produce.defined() && "Pipeline of undefined");
    // update is allowed to be null
    assert(consume.defined() && "Pipeline of undefined");

    Pipeline* node = new Pipeline;
    node->name = name;
    node->produce = produce;
    node->update = update;
    node->consume = consume;
    return node;
  }
};

/// A for loop. Execute the 'body' statement for all values of the
/// variable 'name' from 'min' to 'min + extent'. There are four
/// types of For nodes. A 'Serial' for loop is a conventional
/// one. In a 'Parallel' for loop, each iteration of the loop
/// happens in parallel or in some unspecified order. In a
/// 'Vectorized' for loop, each iteration maps to one SIMD lane,
/// and the whole loop is executed in one shot. For this case,
/// 'extent' must be some small integer constant (probably 4, 8, or
/// 16). An 'Unrolled' for loop compiles to a completely unrolled
/// version of the loop. Each iteration becomes its own
/// statement. Again in this case, 'extent' should be a small
/// integer constant.
struct For : public StmtNode<For> {
  std::string name;
  Expr min, extent;
  typedef enum { Serial, Parallel, Vectorized, Unrolled } ForType;
  ForType for_type;
  Stmt body;

  static Stmt make(std::string name, Expr min, Expr extent, ForType for_type,
                   Stmt body) {
    assert(min.defined() && "For of undefined");
    assert(extent.defined() && "For of undefined");
    assert(min.type().is_scalar() && "For with vector min");
    assert(extent.type().is_scalar() && "For with vector extent");
    assert(body.defined() && "For of undefined");

    For* node = new For;
    node->name = name;
    node->min = min;
    node->extent = extent;
    node->for_type = for_type;
    node->body = body;
    return node;
  }
};

/// Store a 'value' to the buffer called 'name' at a given
/// 'index'. The buffer is interpreted as an array of the same type as
/// 'value'.
struct Store : public StmtNode<Store> {
  std::string name;
  Expr value, index;

  static Stmt make(std::string name, Expr value, Expr index) {
    assert(value.defined() && "Store of undefined");
    assert(index.defined() && "Store of undefined");

    Store* node = new Store;
    node->name = name;
    node->value = value;
    node->index = index;
    return node;
  }
};

/// This defines the value of a function at a multi-dimensional
/// location. You should think of it as a store to a
/// multi-dimensional array. It gets lowered to a conventional
/// Store node.
struct Provide : public StmtNode<Provide> {
  std::string name;
  std::vector<Expr> values;
  std::vector<Expr> args;

  static Stmt make(std::string name, const std::vector<Expr>& values,
                   const std::vector<Expr>& args) {
    assert(!values.empty() && "Provide of no values");
    for (size_t i = 0; i < values.size(); i++) {
      assert(values[i].defined() && "Provide of undefined value");
    }
    for (size_t i = 0; i < args.size(); i++) {
      assert(args[i].defined() && "Provide to undefined location");
    }

    Provide* node = new Provide;
    node->name = name;
    node->values = values;
    node->args = args;
    return node;
  }
};

/// Allocate a scratch area called with the given name, type, and
/// size. The buffer lives for at most the duration of the body
/// statement, within which it is freed. It is an error for an allocate
/// node not to contain a free node of the same buffer.
struct Allocate : public StmtNode<Allocate> {
  std::string name;
  Type type;
  Expr size;
  Stmt body;

  static Stmt make(std::string name, Type type, Expr size, Stmt body) {
    assert(size.defined() && "Allocate of undefined");
    assert(body.defined() && "Allocate of undefined");
    assert(size.type().is_scalar() == 1 && "Allocate of vector size");

    Allocate* node = new Allocate;
    node->name = name;
    node->type = type;
    node->size = size;
    node->body = body;
    return node;
  }
};

/// Free the resources associated with the given buffer.
struct Free : public StmtNode<Free> {
  std::string name;

  static Stmt make(std::string name) {
    Free* node = new Free;
    node->name = name;
    return node;
  }
};

/// A single-dimensional span. Includes all numbers between min and
/// (min + extent - 1)
struct Range {
  Expr min, extent;
  Range() {}
  Range(Expr min, Expr extent) : min(min), extent(extent) {
    assert(min.type() == extent.type() &&
           "Region min and extent must have same type");
  }
};

/// A multi-dimensional box. The outer product of the elements
typedef std::vector<Range> Region;

/// Allocate a multi-dimensional buffer of the given type and
/// size. Create some scratch memory that will back the function 'name'
/// over the range specified in 'bounds'. The bounds are a vector of
/// (min, extent) pairs for each dimension.
struct Realize : public StmtNode<Realize> {
  std::string name;
  std::vector<Type> types;
  Region bounds;
  Stmt body;

  static Stmt make(const std::string& name, const std::vector<Type>& types,
                   const Region& bounds, Stmt body) {
    for (size_t i = 0; i < bounds.size(); i++) {
      assert(bounds[i].min.defined() && "Realize of undefined");
      assert(bounds[i].extent.defined() && "Realize of undefined");
      assert(bounds[i].min.type().is_scalar() && "Realize of vector size");
      assert(bounds[i].extent.type().is_scalar() && "Realize of vector size");
    }
    assert(body.defined() && "Realize of undefined");
    assert(!types.empty() && "Realize has empty type");

    Realize* node = new Realize;
    node->name = name;
    node->types = types;
    node->bounds = bounds;
    node->body = body;
    return node;
  }
};

/// A sequence of statements to be executed in-order. 'rest' may be
/// NULL. Used rest.defined() to find out.
struct Block : public StmtNode<Block> {
  Stmt first, rest;

  static Stmt make(Stmt first, Stmt rest) {
    assert(first.defined() && "Block of undefined");
    // rest is allowed to be null

    Block* node = new Block;
    node->first = first;
    node->rest = rest;
    return node;
  }
};

/// An if-then-else block. 'else' may be NULL.
struct IfThenElse : public StmtNode<IfThenElse> {
  Expr condition;
  Stmt then_case, else_case;

  static Stmt make(Expr condition, Stmt then_case, Stmt else_case = Stmt()) {
    assert(condition.defined() && then_case.defined() &&
           "IfThenElse of undefined");
    // else_case may be null.

    IfThenElse* node = new IfThenElse;
    node->condition = condition;
    node->then_case = then_case;
    node->else_case = else_case;
    return node;
  }
};

/// Evaluate and discard an expression, presumably because it has some
/// side-effect.
struct Evaluate : public StmtNode<Evaluate> {
  Expr value;

  static Stmt make(Expr v) {
    assert(v.defined() && "Evaluate of undefined");

    Evaluate* node = new Evaluate;
    node->value = v;
    return node;
  }
};

}  // namespace internal
}  // namespace jmlang

#endif  // JMLANG_IR_IR_H