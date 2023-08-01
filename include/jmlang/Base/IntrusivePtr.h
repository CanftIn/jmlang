#ifndef JMLANG_BASE_INTRUSIVE_PTR_H
#define JMLANG_BASE_INTRUSIVE_PTR_H

#include <cstdlib>
#include <iostream>

#include "jmlang/Base/Util.h"

namespace jmlang {
namespace internal {

class RefCount {
  int count_;

 public:
  RefCount() : count_(0) {}
  void increment() { count_++; }
  void decrement() { count_--; }
  bool is_zero() const { return count_ == 0; }
};

template <typename T>
RefCount& ref_count(const T*);

template <typename T>
void destroy(const T*);

template <typename T>
struct IntrusivePtr {
 private:
  void incref(T* p) {
    if (p) {
      ref_count(p).increment();
    }
  };

  void decref(T* p) {
    if (p) {
      ref_count(p).decrement();
      if (ref_count(p).is_zero()) {
        // std::cout << "Destroying " << ptr << ", " << live_objects << "\n";
        destroy(p);
      }
    }
  }

 public:
  T* ptr;

  ~IntrusivePtr() { decref(ptr); }

  IntrusivePtr() : ptr(NULL) {}

  IntrusivePtr(T* p) : ptr(p) { incref(ptr); }

  IntrusivePtr(const IntrusivePtr<T>& other) : ptr(other.ptr) { incref(ptr); }

  IntrusivePtr<T>& operator=(const IntrusivePtr<T>& other) {
    T* temp = other.ptr;
    incref(temp);
    decref(ptr);
    ptr = temp;
    return *this;
  }

  /// Handles can be nullptr.
  bool defined() const { return ptr != nullptr; }

  bool same_as(const IntrusivePtr& other) const { return ptr == other.ptr; }
};

}  // namespace internal
}  // namespace jmlang

#endif  // JMLANG_BASE_INTRUSIVE_PTR_H