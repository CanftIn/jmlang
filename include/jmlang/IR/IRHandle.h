#ifndef JMLANG_IR_IRHANDLE_H
#define JMLANG_IR_IRHANDLE_H

#include <string>
#include <vector>

#include "jmlang/Base/IntrusivePtr.h"
#include "jmlang/Base/Type.h"
#include "jmlang/Base/Util.h"
#include "jmlang/IR/IRVisitor.h"

namespace jmlang {
namespace internal {

struct IRNodeType {};

struct IRNode {
  virtual void accept(IRVisitor* v) const = 0;
  IRNode() {}
  virtual ~IRNode() {}

  mutable RefCount ref_count;
  virtual const IRNodeType* type_info() const = 0;
};

template <>
inline RefCount& ref_count<IRNode>(const IRNode* p) {
  return p->ref_count;
}

template <>
inline void destroy<IRNode>(const IRNode* p) {
  delete p;
}

/// IR nodes are split into expressions and statements.
/// expressions like: x + 3
/// statements like: assert(x + 3)
struct BaseStmtNode : public IRNode {};

struct BaseExprNode : public IRNode {
  Type type;
};

/// use CRTP to avoid duplicated code in the IR Nodes.
template <typename T>
struct ExprNode : public BaseExprNode {
  void accept(IRVisitor* v) const { v->visit((const T*)this); }
  virtual IRNodeType* type_info() const { return &type_info_; }
  static IRNodeType type_info_;
};

template <typename T>
struct StmtNode : public BaseStmtNode {
  void accept(IRVisitor* v) const { v->visit((const T*)this); }
  virtual IRNodeType* type_info() const { return &type_info_; }
  static IRNodeType type_info_;
};

struct IRHandle : public IntrusivePtr<const IRNode> {
  IRHandle() : IntrusivePtr<const IRNode>() {}
  IRHandle(const IRNode* p) : IntrusivePtr<const IRNode>(p) {}

  void accept(IRVisitor* v) const { ptr->accept(v); }

  /// Downcast this ir node to its actual type (e.g. Add, or
  /// Select). This returns nullptr if the node is not of the requested
  /// type. Example usage:
  ///
  /// if (const Add *add = node->as<Add>()) {
  ///   // This is an add node
  /// }
  ///
  template <typename T>
  const T* as() const {
    if (ptr->type_info() == &T::type_info_) {
      return (const T*)ptr;
    }
    return nullptr;
  }
};

/// Integer constants, Imm: immortal
struct IntImm : public ExprNode<IntImm> {
  int value;

  static IntImm* make(int value) {
    if (value >= -8 && value <= 8)
      return small_int_cache + value + 8;
    IntImm* node = new IntImm;
    node->type = Int(32);
    node->value = value;
    return node;
  }

 private:
  /// ints from -8 to 8
  static IntImm small_int_cache[17];
};

/// Floating-point constants
struct FloatImm : public ExprNode<FloatImm> {
  float value;

  static FloatImm* make(float value) {
    FloatImm* node = new FloatImm;
    node->type = Float(32);
    node->value = value;
    return node;
  }
};

/// String constants
struct StringImm : public ExprNode<StringImm> {
  std::string value;

  static StringImm* make(const std::string& val) {
    StringImm* node = new StringImm;
    node->type = Handle();
    node->value = val;
    return node;
  }
};

}  // namespace internal

}  // namespace jmlang

#endif  // JMLANG_IR_IRHANDLE_H