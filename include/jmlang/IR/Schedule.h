#ifndef JMLANG_IR_SECHEDULE_H
#define JMLANG_IR_SECHEDULE_H

#include <string>
#include <vector>

#include "jmlang/IR/IR.h"

namespace jmlang {
namespace internal {

/// A schedule for a function, which defines where, when, and
/// how it should be evaluated.
struct Schedule {
  struct LoopLevel {
    std::string func, var;

    LoopLevel(std::string f, std::string v) : func(f), var(v) {}

    LoopLevel() {}

    bool is_inline() const { return var.empty(); }

    /// root is a special LoopLevel value which represents the
    /// location outside of all for loops.
    static LoopLevel root() { return LoopLevel("", "<root>"); }

    bool is_root() const { return var == "<root>"; }

    bool match(const std::string& loop) const {
      return starts_with(loop, func + ".") && ends_with(loop, "." + var);
    }

    bool match(const LoopLevel& other) const {
      return (func == other.func &&
              (var == other.var || ends_with(var, "." + other.var) ||
               ends_with(other.var, "." + var)));
    }
  };

  /// The store_level must be outside
  /// of or equal to the compute_level. If the compute_level is
  /// inline, the store_level is meaningless.
  LoopLevel store_level, compute_level;

  struct Split {
    std::string old_var, outer, inner;
    Expr factor;

    enum SplitType { SplitVar = 0, RenameVar, FuseVars };

    // If split_type is Rename, then this is just a renaming of the
    // old_var to the outer and not a split. The inner var should
    // be ignored, and factor should be one. Renames are kept in
    // the same list as splits so that ordering between them is
    // respected.
    // If split_type is Fuse, then this does the opposite of a
    // split, it joins the outer and inner into the old_var.
    SplitType split_type;

    bool is_rename() const { return split_type == RenameVar; }
    bool is_split() const { return split_type == SplitVar; }
    bool is_fuse() const { return split_type == FuseVars; }
  };

  /// The traversal of the domain of a function can have some of its
  /// dimensions split into sub-dimensions.
  std::vector<Split> splits;

  struct Dim {
    std::string var;
    For::ForType for_type;
  };

  /// The list and ordering of dimensions used to evaluate this
  /// function, after all splits have taken place. The first
  /// dimension in the vector corresponds to the innermost for loop,
  /// and the last is the outermost. Also specifies what type of for
  /// loop to use for each dimension. Does not specify the bounds on
  /// each dimension. These get inferred from how the function is
  /// used, what the splits are, and any optional bounds in the list below.
  std::vector<Dim> dims;

  /// The list and order of dimensions used to store this
  /// function. The first dimension in the vector corresponds to the
  /// innermost dimension for storage (i.e. which dimension is
  /// tightly packed in memory)
  std::vector<std::string> storage_dims;

  struct Bound {
    std::string var;
    Expr min, extent;
  };

  /// You may explicitly bound some of the dimensions of a function. 
  std::vector<Bound> bounds;
};

}  // namespace internal
}  // namespace jmlang

#endif  // JMLANG_IR_SECHEDULE_H