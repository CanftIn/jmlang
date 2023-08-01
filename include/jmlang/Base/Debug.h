#ifndef JMLANG_BASE_DEBUG_H
#define JMLANG_BASE_DEBUG_H

#include <iostream>
#include <string>

#include "jmlang/IR/IR.h"

namespace jmlang {
namespace internal {

/// For optional debugging during codegen, use the debug class as
/// follows:
///
///  debug(verbosity) << "The expression is " << expr << std::endl;
///
/// 0 always prints,
/// 1 should print after every major stage,
/// 2 should be used for more detail,
/// 3 should be used for tracing everything that occurs.
///
struct debug {
  static int debug_level;
  static bool initialized;
  int verbosity;

  debug(int v) : verbosity(v) {
    if (!initialized) {
      if (char* lvl = getenv("JM_DEBUG_CODEGEN")) {
        debug_level = atoi(lvl);
      } else {
        debug_level = 0;
      }

      initialized = true;
    }
  }

  template <typename T>
  debug& operator<<(T x) {
    if (verbosity > debug_level)
      return *this;
    std::cerr << x;
    return *this;
  }
};

int debug::debug_level = 0;
bool debug::initialized = false;

}  // namespace internal
}  // namespace jmlang

#endif  // JMLANG_BASE_DEBUG_H