#include "jmlang/IR/Parameter.h"

#include "jmlang/IR/IR.h"

namespace jmlang {
namespace internal {

template <>
RefCount& ref_count<ParameterContents>(const ParameterContents* p) {
  return p->ref_count;
}

template <>
void destroy<ParameterContents>(const ParameterContents* p) {
  delete p;
}

}  // namespace internal
}  // namespace jmlang