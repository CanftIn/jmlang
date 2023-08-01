#ifndef JMLANG_IR_REDUCTION_H
#define JMLANG_IR_REDUCTION_H

#include <string>
#include <vector>

#include "jmlang/Base/IntrusivePtr.h"
#include "jmlang/IR/Expr.h"

namespace jmlang {
namespace internal {

/// A single named dimension of a reduction domain.
struct ReductionVariable {
  std::string var;
  Expr min, extent;
};

struct ReductionDomainContents {
  mutable RefCount ref_count;
  std::vector<ReductionVariable> domain;
};

/// A reference-counted handle on a reduction domain, which is just a
/// vector of ReductionVariable.
class ReductionDomain {
  IntrusivePtr<ReductionDomainContents> contents;

 public:
  ReductionDomain() : contents(nullptr) {}

  /// Construct a reduction domain that spans the outer product of
  /// all values of the given ReductionVariable in scanline order,
  /// with the start of the vector being innermost, and the end of
  /// the vector being outermost.
  ReductionDomain(const std::vector<ReductionVariable>& domain)
      : contents(new ReductionDomainContents) {
    contents.ptr->domain = domain;
  }

  /// Is this handle non-NULL.
  bool defined() const { return contents.defined(); }

  /// Tests for equality of reference. Only one reduction domain is
  /// allowed per reduction function, and this is used to verify that.
  bool same_as(const ReductionDomain& other) const {
    return contents.same_as(other.contents);
  }

  /// Immutable access to the reduction variables.
  const std::vector<ReductionVariable>& domain() const {
    return contents.ptr->domain;
  }
};

}  // namespace internal
}  // namespace jmlang

#endif  // JMLANG_IR_REDUCTION_H