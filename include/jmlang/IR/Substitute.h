#ifndef JMLANG_IR_SUBSTITUTE_H
#define JMLANG_IR_SUBSTITUTE_H

#include <map>

#include "jmlang/IR/IRMutator.h"

namespace jmlang {
namespace internal {

/// Substitute variables with the given name with the replacement
/// expression within expr. This is a dangerous thing to do if variable
/// names have not been uniquified. While it won't traverse inside let
/// statements with the same name as the first argument, moving a piece
/// of syntax around can change its meaning, because it can cross lets
/// that redefine variable names that it includes references to.
Expr substitute(std::string name, Expr replacement, Expr expr);

/// Substitute variables with the given name with the replacement
/// expression within stmt.
Stmt substitute(std::string name, Expr replacement, Stmt stmt);

/// Substitute variables with names in the map.
Expr substitute(const std::map<std::string, Expr>& replacements, Expr expr);
Stmt substitute(const std::map<std::string, Expr>& replacements, Stmt stmt);

}  // namespace internal
}  // namespace jmlang

#endif  // JMLANG_IR_SUBSTITUTE_H