#include "jmlang/Base/Type.h"

#include "jmlang/Base/Util.h"
#include "jmlang/IR/IR.h"
#include "jmlang/IR/ExprCall.h"

#include <sstream>

namespace jmlang {

int Type::imax() const {
  if (is_uint()) {
    if (bits == 32) {
      return 0xffffffff;
    } else if (bits < 32) {
      return (int)((1 << bits) - 1);
    } else {
      assert(0 && "max of Type: Type is too large");
      return 0;
    }
  } else if (is_int()) {
    if (bits == 32) {
      return 0x7fffffff;
    } else if (bits < 32) {
      return (int)((1 << (bits - 1)) - 1);
    } else {
      assert(0 && "max of Type: Type is too large");
      return 0;
    }
  } else {
    assert(0 && "max of Type: Not available for floating point types");
    return 0;
  }
}

jmlang::Expr Type::max() const {
  if (width > 1) {
    return internal::Broadcast::make(element_of().max(), width);
  }
  if (is_int() && bits == 32) {
    return imax();  // No explicit cast of scalar i32.
  } else if ((is_int() || is_uint()) && bits <= 32) {
    return internal::Cast::make(*this, imax());
  } else {
    // Use a run-time call to a math intrinsic (see posix_math.cpp)
    std::ostringstream ss;
    ss << "maxval_";
    if (is_int())
      ss << "s";
    else if (is_uint())
      ss << "u";
    else
      ss << "f";
    ss << bits;
    return internal::Call::make(*this, ss.str(), std::vector<Expr>(),
                                internal::Call::Extern);
  }
}

int Type::imin() const {
  if (is_uint()) {
    return 0;
  } else if (is_int()) {
    if (bits == 32) {
      return 0x80000000;
    } else if (bits < 32) {
      return -(1 << (bits - 1));
    } else {
      assert(0 && "min of Type: Type is too large");
      return 0;
    }
  } else {
    // Use a run-time call to a math intrinsic
    assert(0 && "min of Type: Not available for floating point types");
    return 0;
  }
}

Expr Type::min() const {
  if (width > 1) {
    return internal::Broadcast::make(element_of().min(), width);
  }
  if (is_int() && bits == 32) {
    return imin();  // No explicit cast of scalar i32.
  } else if ((is_int() || is_uint()) && bits <= 32) {
    return internal::Cast::make(*this, imin());
  } else {
    // Use a run-time call to a math intrinsic (see posix_math.cpp)
    std::ostringstream ss;
    ss << "minval_";
    if (is_int())
      ss << "s";
    else if (is_uint())
      ss << "u";
    else
      ss << "f";
    ss << bits;
    return internal::Call::make(*this, ss.str(), std::vector<Expr>(),
                                internal::Call::Extern);
  }
}

}  // namespace jmlang