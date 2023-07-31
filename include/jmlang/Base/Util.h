#ifndef JMLANG_BASE_UTIL_H
#define JMLANG_BASE_UTIL_H

#include <cassert>
#include <cstring>
#include <string>
#include <vector>

namespace jmlang {
namespace internal {

template <typename T>
std::vector<T> vec(T a) {
  std::vector<T> v(1);
  v[0] = a;
  return v;
}

template <typename T>
std::vector<T> vec(T a, T b) {
  std::vector<T> v(2);
  v[0] = a;
  v[1] = b;
  return v;
}

template <typename T>
std::vector<T> vec(T a, T b, T c) {
  std::vector<T> v(3);
  v[0] = a;
  v[1] = b;
  v[2] = c;
  return v;
}

template <typename T>
std::vector<T> vec(T a, T b, T c, T d) {
  std::vector<T> v(4);
  v[0] = a;
  v[1] = b;
  v[2] = c;
  v[3] = d;
  return v;
}

template <typename T>
std::vector<T> vec(T a, T b, T c, T d, T e) {
  std::vector<T> v(5);
  v[0] = a;
  v[1] = b;
  v[2] = c;
  v[3] = d;
  v[4] = e;
  return v;
}

template <typename T>
std::vector<T> vec(T a, T b, T c, T d, T e, T f) {
  std::vector<T> v(6);
  v[0] = a;
  v[1] = b;
  v[2] = c;
  v[3] = d;
  v[4] = e;
  v[5] = f;
  return v;
}

/// Convert an integer to a string.
std::string int_to_string(int x);

/// An aggressive form of reinterpret cast.
template <typename DstType, typename SrcType>
DstType reinterpret_bits(const SrcType& src) {
  assert(sizeof(SrcType) == sizeof(DstType));
  DstType dst;
  memcpy(&dst, &src, sizeof(SrcType));
  return dst;
}

/// Generate a unique name starting with the given character.
std::string unique_name(char prefix);

/// Generate a unique name starting with the given string.
std::string unique_name(const std::string& name);

/// true if the first string starts with the second string.
bool starts_with(const std::string& str, const std::string& prefix);

/// true if the first string ends with the second string.
bool ends_with(const std::string& str, const std::string& suffix);

/// Return the final token of the name string.
std::string base_name(const std::string& name);
}  // namespace internal
}  // namespace jmlang

#endif  // JMLANG_BASE_UTIL_H