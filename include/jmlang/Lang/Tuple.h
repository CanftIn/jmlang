#ifndef JMLANG_LANG_TUPLE_H
#define JMLANG_LANG_TUPLE_H

#include <vector>

#include "jmlang/Base/Util.h"
#include "jmlang/IR/IR.h"
#include "jmlang/IR/IROperator.h"

namespace jmlang {

class FuncRefVar;
class FuncRefExpr;

/// Create a small array of Exprs for defining and calling functions
/// with multiple outputs.
class Tuple {
 private:
  std::vector<Expr> exprs;

 public:
  /// The number of elements in the tuple.
  size_t size() const { return exprs.size(); }

  /// Get a reference to an element.
  Expr& operator[](size_t x) {
    assert((x < exprs.size()) && "Tuple access out of bounds");
    return exprs[x];
  }

  /// Get a copy of an element.
  Expr operator[](size_t x) const {
    assert((x < exprs.size()) && "Tuple access out of bounds");
    return exprs[x];
  }

  /// Construct a Tuple from some Exprs.
  Tuple(Expr a, Expr b) : exprs(internal::vec<Expr>(a, b)) {}

  Tuple(Expr a, Expr b, Expr c) : exprs(internal::vec<Expr>(a, b, c)) {}

  Tuple(Expr a, Expr b, Expr c, Expr d)
      : exprs(internal::vec<Expr>(a, b, c, d)) {}

  Tuple(Expr a, Expr b, Expr c, Expr d, Expr e)
      : exprs(internal::vec<Expr>(a, b, c, d, e)) {}

  Tuple(Expr a, Expr b, Expr c, Expr d, Expr e, Expr f)
      : exprs(internal::vec<Expr>(a, b, c, d, e, f)) {}

  /// Construct a Tuple from a vector of Exprs.
  explicit Tuple(const std::vector<Expr>& e) : exprs(e) {
    assert((e.size() > 0) && "Tuples must have at least one element\n");
  }

  /// Construct a Tuple from a function reference.
  Tuple(const FuncRefVar&);
  Tuple(const FuncRefExpr&);

  /// Treat the tuple as a vector of Exprs.
  const std::vector<Expr>& as_vector() const { return exprs; }
};

/// Funcs with Tuple values return multiple buffers when you realize
/// them. Tuples are to Exprs as Realizations are to Buffers.
class Realization {
 private:
  std::vector<Buffer> buffers;

 public:
  /// The number of elements in the tuple.
  size_t size() const { return buffers.size(); }

  /// Get a reference to an element.
  Buffer& operator[](size_t x) {
    assert(x < buffers.size() && "Realization access out of bounds");
    return buffers[x];
  }

  /// Get a copy of an element.
  Buffer operator[](size_t x) const {
    assert((x < buffers.size()) && "Realization access out of bounds");
    return buffers[x];
  }

  /// Single-element realizations are implicitly castable to Buffers.
  operator Buffer() const {
    assert((buffers.size() == 1) &&
           "Can only cast single-element realizations to buffers or images");
    return buffers[0];
  }

  Realization(Buffer a, Buffer b) : buffers(internal::vec<Buffer>(a, b)) {}

  Realization(Buffer a, Buffer b, Buffer c)
      : buffers(internal::vec<Buffer>(a, b, c)) {}

  Realization(Buffer a, Buffer b, Buffer c, Buffer d)
      : buffers(internal::vec<Buffer>(a, b, c, d)) {}

  Realization(Buffer a, Buffer b, Buffer c, Buffer d, Buffer e)
      : buffers(internal::vec<Buffer>(a, b, c, d, e)) {}

  Realization(Buffer a, Buffer b, Buffer c, Buffer d, Buffer e, Buffer f)
      : buffers(internal::vec<Buffer>(a, b, c, d, e, f)) {}

  /// Construct a Realization from a vector of Buffers.
  explicit Realization(const std::vector<Buffer>& e) : buffers(e) {
    assert(e.size() > 0 && "Realizations must have at least one element\n");
  }

  /// Treat the Realization as a vector of Buffers.
  const std::vector<Buffer>& as_vector() const { return buffers; }
};

/// Equivalents of some standard operators for tuples.
inline Tuple tuple_select(Tuple condition, const Tuple& true_value,
                          const Tuple& false_value) {
  Tuple result(std::vector<Expr>(condition.size()));
  for (size_t i = 0; i < result.size(); i++) {
    result[i] = select(condition[i], true_value[i], false_value[i]);
  }
  return result;
}

inline Tuple tuple_select(Expr condition, const Tuple& true_value,
                          const Tuple& false_value) {
  Tuple result(std::vector<Expr>(true_value.size()));
  for (size_t i = 0; i < result.size(); i++) {
    result[i] = select(condition, true_value[i], false_value[i]);
  }
  return result;
}

}  // namespace jmlang

#endif  // JMLANG_LANG_TUPLE_H