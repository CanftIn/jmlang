#ifndef JMLANG_BASE_ARGUMENT_H
#define JMLANG_BASE_ARGUMENT_H

#include <string>

#include "jmlang/Base/Type.h"

namespace jmlang {

/// An argument to a generated function. Used for specifying
/// the function signature of generated code.
struct Argument {
  std::string name;

  /// An argument is either a primitive type (for parameters), or a
  /// buffer pointer. If 'is_buffer' is true, then 'type' should be
  /// ignored.
  bool is_buffer;

  /// If this is a scalar parameter, then this is its types
  Type type;

  Argument() : is_buffer(false) {}
  Argument(const std::string& _name, bool _is_buffer, Type _type)
      : name(_name), is_buffer(_is_buffer), type(_type) {}
};

}  // namespace jmlang

#endif  // JMLANG_BASE_ARGUMENT_H