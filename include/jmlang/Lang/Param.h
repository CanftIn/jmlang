#ifndef JMLANG_LANG_PARAM_H
#define JMLANG_LANG_PARAM_H

#include <sstream>
#include <vector>

#include "jmlang/IR/IR.h"
#include "jmlang/IR/IRPrinter.h"
#include "jmlang/Lang/Var.h"

namespace jmlang {

/** A scalar parameter to a halide pipeline. If you're jitting, this
 * should be bound to an actual value of type T using the set method
 * before you realize the function uses this. If you're statically
 * compiling, this param should appear in the argument list. */
template <typename T>
class Param {
  /** A reference-counted handle on the internal parameter object */
  internal::Parameter param;

 public:
  /** Construct a scalar parameter of type T with a unique
   * auto-generated name */
  Param() : param(type_of<T>(), false) {}

  /** Construct a scalar parameter of type T with the given name */
  Param(const std::string& n) : param(type_of<T>(), false, n) {}

  /** Get the name of this parameter */
  const std::string& name() const { return param.name(); }

  /** Get the current value of this parameter. Only meaningful when jitting. */
  T get() const { return param.get_scalar<T>(); }

  /** Set the current value of this parameter. Only meaningful when jitting */
  void set(T val) { param.set_scalar<T>(val); }

  /** Get the halide type of T */
  Type type() const { return type_of<T>(); }

  /** Get or set the possible range of this parameter. Use undefined
   * Exprs to mean unbounded. */
  // @{
  void set_range(Expr min, Expr max) {
    set_min_value(min);
    set_max_value(max);
  }

  void set_min_value(Expr min) {
    if (min.type() != type_of<T>()) {
      min = internal::Cast::make(type_of<T>(), min);
    }
    param.set_min_value(min);
  }

  void set_max_value(Expr max) {
    if (max.type() != type_of<T>()) {
      max = internal::Cast::make(type_of<T>(), max);
    }
    param.set_max_value(max);
  }

  Expr get_min_value() { return param.get_min_value(); }

  Expr get_max_value() { return param.get_max_value(); }
  // @}

  /** You can use this parameter as an expression in a halide
   * function definition */
  operator Expr() const {
    return internal::Variable::make(type_of<T>(), name(), param);
  }

  /** Using a param as the argument to an external stage treats it
   * as an Expr */
  operator ExternFuncArgument() const { return Expr(*this); }

  /** Construct the appropriate argument matching this parameter,
   * for the purpose of generating the right type signature when
   * statically compiling halide pipelines. */
  operator Argument() const { return Argument(name(), false, type()); }
};

/** A handle on the output buffer of a pipeline. Used to make static
 * promises about the output size and stride. */
class OutputImageParam {
 protected:
  /** A reference-counted handle on the internal parameter object */
  internal::Parameter param;

  /** The dimensionality of this image. */
  int dims;

  bool add_implicit_args_if_placeholder(std::vector<Expr>& args, Expr last_arg,
                                        int total_args,
                                        bool placeholder_seen) const {
    const internal::Variable* var = last_arg.as<internal::Variable>();
    bool is_placeholder = var != NULL && Var::is_placeholder(var->name);
    if (is_placeholder) {
      assert(!placeholder_seen &&
             "Only one implicit placeholder ('_') allowed in argument list for "
             "ImageParam.");
      placeholder_seen = true;

      // The + 1 in the conditional is because one provided argument is an
      // placeholder
      for (int i = 0; i < (dims - total_args + 1); i++) {
        args.push_back(Var::implicit(i));
      }
    } else {
      args.push_back(last_arg);
    }

    return is_placeholder;
  }

 public:
  /** Construct a NULL image parameter handle. */
  OutputImageParam() : dims(0) {}

  /** Construct an OutputImageParam that wraps an internal Parameter object. */
  OutputImageParam(const internal::Parameter& p, int d) : param(p), dims(d) {}

  /** Get the name of this Param */
  const std::string& name() const { return param.name(); }

  /** Get the type of the image data this Param refers to */
  Type type() const { return param.type(); }

  /** Is this parameter handle non-NULL */
  bool defined() { return param.defined(); }

  /** Get an expression representing the extent of this image
   * parameter in the given dimension */
  Expr extent(int x) const {
    std::ostringstream s;
    s << name() << ".extent." << x;
    return internal::Variable::make(Int(32), s.str(), param);
  }

  /** Get an expression representing the stride of this image in the
   * given dimension */
  Expr stride(int x) const {
    std::ostringstream s;
    s << name() << ".stride." << x;
    return internal::Variable::make(Int(32), s.str(), param);
  }

  /** Set the extent in a given dimension to equal the given
   * expression. Images passed in that fail this check will generate
   * a runtime error. Returns a reference to the ImageParam so that
   * these calls may be chained.
   *
   * This may help the compiler generate better
   * code. E.g:
   \code
   im.set_extent(0, 100);
   \endcode
   * tells the compiler that dimension zero must be of extent 100,
   * which may result in simplification of boundary checks. The
   * value can be an arbitrary expression:
   \code
   im.set_extent(0, im.extent(1));
   \endcode
   * declares that im is a square image (of unknown size), whereas:
   \code
   im.set_extent(0, (im.extent(0)/32)*32);
   \endcode
   * tells the compiler that the extent is a multiple of 32. */
  OutputImageParam& set_extent(int dim, Expr extent) {
    param.set_extent_constraint(dim, extent);
    return *this;
  }

  /** Set the min in a given dimension to equal the given
   * expression. Setting the mins to zero may simplify some
   * addressing math. */
  OutputImageParam& set_min(int dim, Expr min) {
    param.set_min_constraint(dim, min);
    return *this;
  }

  /** Set the stride in a given dimension to equal the given
   * value. This is particularly helpful to set when
   * vectorizing. Known strides for the vectorized dimension
   * generate better code. */
  OutputImageParam& set_stride(int dim, Expr stride) {
    param.set_stride_constraint(dim, stride);
    return *this;
  }

  /** Set the min and extent in one call. */
  OutputImageParam& set_bounds(int dim, Expr min, Expr extent) {
    return set_min(dim, min).set_extent(dim, extent);
  }

  /** Get the dimensionality of this image parameter */
  int dimensions() const { return dims; };

  /** Get an expression giving the extent in dimension 0, which by
   * convention is the width of the image */
  Expr width() const {
    assert(dims >= 0);
    return extent(0);
  }

  /** Get an expression giving the extent in dimension 1, which by
   * convention is the height of the image */
  Expr height() const {
    assert(dims >= 1);
    return extent(1);
  }

  /** Get an expression giving the extent in dimension 2, which by
   * convention is the channel-count of the image */
  Expr channels() const {
    assert(dims >= 2);
    return extent(2);
  }

  /** Get at the internal parameter object representing this ImageParam. */
  internal::Parameter parameter() const { return param; }

  /** Construct the appropriate argument matching this parameter,
   * for the purpose of generating the right type signature when
   * statically compiling halide pipelines. */
  operator Argument() const { return Argument(name(), true, type()); }
};

/** An Image parameter to a halide pipeline. E.g., the input image. */
class ImageParam : public OutputImageParam {
 public:
  /** Construct a NULL image parameter handle. */
  ImageParam() : OutputImageParam() {}

  /** Construct an image parameter of the given type and
   * dimensionality, with an auto-generated unique name. */
  ImageParam(Type t, int d)
      : OutputImageParam(internal::Parameter(t, true), d) {}

  /** Construct an image parameter of the given type and
   * dimensionality, with the given name */
  ImageParam(Type t, int d, const std::string& n)
      : OutputImageParam(internal::Parameter(t, true, n), d) {}

  /** Bind a buffer or image to this ImageParam. Only relevant for jitting */
  void set(Buffer b) {
    if (b.defined())
      assert(b.type() == type() && "Setting buffer of incorrect type");
    param.set_buffer(b);
  }

  /** Get the buffer bound to this ImageParam. Only relevant for jitting */
  Buffer get() const { return param.get_buffer(); }

  /** Construct an expression which loads from this image
   * parameter. The location is extended with enough implicit
   * variables to match the dimensionality of the image
   * (see \ref Var::implicit)
   */
  // @{
  Expr operator()() const {
    assert(dimensions() == 0);
    std::vector<Expr> args;
    return internal::Call::make(param, args);
  }

  /** Force the args to a call to an image to be int32. */
  static void check_arg_types(const std::string& name,
                              std::vector<Expr>* args) {
    for (size_t i = 0; i < args->size(); i++) {
      Type t = (*args)[i].type();
      if (t.is_float() || (t.is_uint() && t.bits >= 32) ||
          (t.is_int() && t.bits > 32)) {
        std::cerr << "Error: implicit cast from " << t << " to int in argument "
                  << (i + 1) << " in call to " << name
                  << " is not allowed. Use an explicit cast.\n";
        assert(false);
      }
      // We're allowed to implicitly cast from other varieties of int
      if (t != Int(32)) {
        (*args)[i] = internal::Cast::make(Int(32), (*args)[i]);
      }
    }
  }

  Expr operator()(Expr x) const {
    std::vector<Expr> args;
    bool placeholder_seen = false;
    placeholder_seen |=
        add_implicit_args_if_placeholder(args, x, 1, placeholder_seen);

    assert(args.size() == (size_t)dims);

    check_arg_types(name(), &args);
    return internal::Call::make(param, args);
  }

  Expr operator()(Expr x, Expr y) const {
    std::vector<Expr> args;
    bool placeholder_seen = false;
    placeholder_seen |=
        add_implicit_args_if_placeholder(args, x, 2, placeholder_seen);
    placeholder_seen |=
        add_implicit_args_if_placeholder(args, y, 2, placeholder_seen);

    assert(args.size() == (size_t)dims);

    check_arg_types(name(), &args);
    return internal::Call::make(param, args);
  }

  Expr operator()(Expr x, Expr y, Expr z) const {
    std::vector<Expr> args;
    bool placeholder_seen = false;
    placeholder_seen |=
        add_implicit_args_if_placeholder(args, x, 3, placeholder_seen);
    placeholder_seen |=
        add_implicit_args_if_placeholder(args, y, 3, placeholder_seen);
    placeholder_seen |=
        add_implicit_args_if_placeholder(args, z, 3, placeholder_seen);

    assert(args.size() == (size_t)dims);

    check_arg_types(name(), &args);
    return internal::Call::make(param, args);
  }

  Expr operator()(Expr x, Expr y, Expr z, Expr w) const {
    std::vector<Expr> args;
    bool placeholder_seen = false;
    placeholder_seen |=
        add_implicit_args_if_placeholder(args, x, 4, placeholder_seen);
    placeholder_seen |=
        add_implicit_args_if_placeholder(args, y, 4, placeholder_seen);
    placeholder_seen |=
        add_implicit_args_if_placeholder(args, z, 4, placeholder_seen);
    placeholder_seen |=
        add_implicit_args_if_placeholder(args, w, 4, placeholder_seen);

    assert(args.size() == (size_t)dims);

    check_arg_types(name(), &args);
    return internal::Call::make(param, args);
  }
  // @}

  /** Treating the image parameter as an Expr is equivalent to call
   * it with no arguments. For example, you can say:
   *
   \code
   ImageParam im(UInt(8), 2);
   Func f;
   f = im*2;
   \endcode
   *
   * This will define f as a two-dimensional function with value at
   * position (x, y) equal to twice the value of the image parameter
   * at the same location.
   */
  operator Expr() const { return (*this)(_); }
};

}  // namespace jmlang

#endif  // JMLANG_LANG_PARAM_H