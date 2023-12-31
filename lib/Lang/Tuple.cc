#include "jmlang/Lang/Tuple.h"

#include "jmlang/Lang/Func.h"

namespace jmlang {

Tuple::Tuple(const FuncRefVar& f) : exprs(f.size()) {
  assert(f.size() > 1 &&
         "Can only construct Tuples from calls to functions with more than one "
         "output");
  for (size_t i = 0; i < f.size(); i++) {
    exprs[i] = f[i];
  }
}

Tuple::Tuple(const FuncRefExpr& f) : exprs(f.size()) {
  assert(f.size() > 1 &&
         "Can only construct Tuples from calls to functions with more than one "
         "output");
  for (size_t i = 0; i < f.size(); i++) {
    exprs[i] = f[i];
  }
}

}  // namespace jmlang