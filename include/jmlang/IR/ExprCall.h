#ifndef JMLANG_IR_EXPR_CALL_H
#define JMLANG_IR_EXPR_CALL_H

#include "jmlang/IR/Function.h"

namespace jmlang {
namespace internal {

/// A function call. This can represent a call to some extern
/// function (like sin), but it's also our multi-dimensional
/// version of a Load, so it can be a load from an input image, or
/// a call to another jmlang function. The latter two types of call
/// nodes don't survive all the way down to code generation - the
/// lowering process converts them to Load nodes.
struct Call : public ExprNode<Call> {
  std::string name;
  std::vector<Expr> args;
  typedef enum { Image, Extern, Jmlang, Intrinsic } CallType;
  CallType call_type;

  static const std::string debug_to_file,
      shuffle_vector,
      interleave_vectors,
      reinterpret,
      bitwise_and,
      bitwise_not,
      bitwise_xor,
      bitwise_or,
      shift_left,
      shift_right,
      rewrite_buffer,
      profiling_timer,
      lerp,
      create_buffer_t,
      extract_buffer_min,
      extract_buffer_extent,
      trace;

  // If it's a call to another function, this call node
  // holds onto a pointer to that function.
  Function func;

  // If that function has multiple values, which value does this
  // call node refer to?
  int value_index;

  // If it's a call to an image, this call nodes hold a
  // pointer to that image's buffer.
  Buffer image;

  // If it's a call to an image parameter, this call nodes holds a
  // pointer to that.
  Parameter param;

  static Expr make(Type type, std::string name, const std::vector<Expr>& args,
                   CallType call_type, Function func = Function(),
                   int value_index = 0, Buffer image = Buffer(),
                   Parameter param = Parameter()) {
    for (size_t i = 0; i < args.size(); i++) {
      assert(args[i].defined() && "Call of undefined");
    }
    if (call_type == Jmlang) {
      assert(value_index >= 0 && value_index < func.outputs() &&
             "Value index out of range in call to jmlang function");
      assert((func.has_pure_definition() || func.has_extern_definition()) &&
             "Call to undefined jmlang function");
      assert((int)args.size() <= func.dimensions() &&
             "Call node with too many arguments.");
      for (size_t i = 0; i < args.size(); i++) {
        assert(args[i].type() == Int(32) &&
               "Args to call to jmlang function must be type Int(32)");
      }
    } else if (call_type == Image) {
      assert((param.defined() || image.defined()) &&
             "Call node to undefined image");
      for (size_t i = 0; i < args.size(); i++) {
        assert(args[i].type() == Int(32) &&
               "Args to load from image must be type Int(32)");
      }
    }

    Call* node = new Call;
    node->type = type;
    node->name = name;
    node->args = args;
    node->call_type = call_type;
    node->func = func;
    node->value_index = value_index;
    node->image = image;
    node->param = param;
    return node;
  }

  /// Convenience constructor for calls to other functions.
  static Expr make(Function func, const std::vector<Expr>& args, int idx = 0) {
    assert(idx >= 0 && idx < func.outputs() &&
           "Value index out of range in call to jmlang function");
    assert((func.has_pure_definition() || func.has_extern_definition()) &&
           "Call to undefined jmlang function");
    return make(func.output_types()[idx], func.name(), args, Jmlang, func, idx,
                Buffer(), Parameter());
  }

  /// Convenience constructor for loads from concrete images.
  static Expr make(Buffer image, const std::vector<Expr>& args) {
    return make(image.type(), image.name(), args, Image, Function(), 0, image,
                Parameter());
  }

  /// Convenience constructor for loads from images parameters.
  static Expr make(Parameter param, const std::vector<Expr>& args) {
    return make(param.type(), param.name(), args, Image, Function(), 0,
                Buffer(), param);
  }
};

}  // namespace internal
}  // namespace jmlang

#endif  // JMLANG_IR_EXPR_CALL_H