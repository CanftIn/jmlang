#ifndef JMLANG_BASE_TYPE_H
#define JMLANG_BASE_TYPE_H

#include <cstdint>

namespace jmlang {

struct Expr;

struct Type {
  enum TypeKind {
    Int,    // signed integer type
    UInt,   // unsigned integer type
    Float,  // floating-point type
    Handle  // opaque pointer type (i.e. void *)
  } type_kind;

  int bits;  // number of bits in the type

  int width;  // number of elements in the type

  int bytes() const { return (bits + 7) / 8; }

  bool is_bool() const { return type_kind == UInt && bits == 1; }

  bool is_vector() const { return width > 1; }

  bool is_scalar() const { return width == 1; }

  bool is_float() const { return type_kind == Float; }

  bool is_int() const { return type_kind == Int; }

  bool is_uint() const { return type_kind == UInt; }

  bool is_handle() const { return type_kind == Handle; }

  bool operator==(const Type& other) const {
    return type_kind == other.type_kind && bits == other.bits &&
           width == other.width;
  }

  bool operator!=(const Type& other) const { return !(*this == other); }

  /// Produce a vector of this type.
  Type vector_of(int w) const {
    Type type = {type_kind, bits, w};
    return type;
  }

  /// Produce the type of a single element of this vector type.
  Type element_of() const {
    Type type = {type_kind, bits, 1};
    return type;
  }

  /// Return an integer which is the maximum value of this type.
  int imax() const;

  /// Return an expression which is the maximum value of this type
  Expr max() const;

  /// Return an integer which is the minimum value of this type.
  int imin() const;

  /// Return an expression which is the minimum value of this type
  Expr min() const;
};

/// Constructing a signed integer type.
inline Type Int(int bits, int width = 1) {
  Type t;
  t.type_kind = Type::Int;
  t.bits = bits;
  t.width = width;
  return t;
}

/// Constructing an unsigned integer type.
inline Type UInt(int bits, int width = 1) {
  Type t;
  t.type_kind = Type::UInt;
  t.bits = bits;
  t.width = width;
  return t;
}

/// Constructing a floating-point type.
inline Type Float(int bits, int width = 1) {
  Type t;
  t.type_kind = Type::Float;
  t.bits = bits;
  t.width = width;
  return t;
}

/// Constructing a boolean type.
inline Type Bool(int width = 1) { return UInt(1, width); }

/// Constructing a handle type.
inline Type Handle(int width = 1) {
  Type t;
  t.type_kind = Type::Handle;
  t.bits = 64;  // All handles are 64-bit for now
  t.width = width;
  return t;
}

template <typename T>
struct type_of_helper;

template <typename T>
struct type_of_helper<T*> {
  operator Type() { return Handle(); }
};

template <>
struct type_of_helper<float> {
  operator Type() { return Float(32); }
};

template <>
struct type_of_helper<double> {
  operator Type() { return Float(64); }
};

template <>
struct type_of_helper<uint8_t> {
  operator Type() { return UInt(8); }
};

template <>
struct type_of_helper<uint16_t> {
  operator Type() { return UInt(16); }
};

template <>
struct type_of_helper<uint32_t> {
  operator Type() { return UInt(32); }
};

template <>
struct type_of_helper<uint64_t> {
  operator Type() { return UInt(64); }
};

template <>
struct type_of_helper<int8_t> {
  operator Type() { return Int(8); }
};

template <>
struct type_of_helper<int16_t> {
  operator Type() { return Int(16); }
};

template <>
struct type_of_helper<int32_t> {
  operator Type() { return Int(32); }
};

template <>
struct type_of_helper<int64_t> {
  operator Type() { return Int(64); }
};

template <>
struct type_of_helper<bool> {
  operator Type() { return Bool(); }
};

/** Construct the halide equivalent of a C type */
template <typename T>
Type type_of() {
  return Type(type_of_helper<T>());
}

}  // namespace jmlang

#endif  // JMLANG_BASE_TYPE_H