#include "jmlang/IR/Reduction.h"

#include <string>

#include "jmlang/IR/IR.h"

namespace jmlang {
namespace internal {

template <>
RefCount& ref_count<ReductionDomainContents>(const ReductionDomainContents* p) {
  return p->ref_count;
}

template <>
void destroy<ReductionDomainContents>(const ReductionDomainContents* p) {
  delete p;
}

}  // namespace internal
}  // namespace jmlang