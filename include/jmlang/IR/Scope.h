#ifndef JMLANG_IR_SCOPE_H
#define JMLANG_IR_SCOPE_H

#include <iostream>
#include <map>
#include <stack>
#include <string>
#include <utility>

#include "jmlang/Base/Debug.h"
#include "jmlang/Base/Util.h"

namespace jmlang {
namespace internal {

template <typename T>
class SmallStack {
 private:
  T _top;
  std::vector<T> _rest;
  bool _empty;

 public:
  SmallStack() : _empty(true) {}

  void pop() {
    assert(!_empty);
    if (_rest.empty()) {
      _empty = true;
      _top = T();
    } else {
      _top = _rest.back();
      _rest.pop_back();
    }
  }

  void push(const T& t) {
    if (_empty) {
      _empty = false;
    } else {
      _rest.push_back(_top);
    }
    _top = t;
  }

  T top() const {
    assert(!_empty);
    return _top;
  }

  T& top_ref() {
    assert(!_empty);
    return _top;
  }

  bool empty() const { return _empty; }
};

/// A common pattern when traversing IR is that you need to
/// keep track of stuff when you find a Let or a LetStmt, and that it
/// should hide previous values with the same name until you leave the
/// Let or LetStmt nodes This class helps with that.
template <typename T>
class Scope {
 private:
  std::map<std::string, SmallStack<T> > table;

 public:
  Scope() {}

  /// Retrive the value referred to by a name.
  T get(const std::string& name) const {
    typename std::map<std::string, SmallStack<T> >::const_iterator iter =
        table.find(name);
    if (iter == table.end() || iter->second.empty()) {
      std::cerr << "Symbol '" << name << "' not found" << std::endl;
      assert(false);
    }
    return iter->second.top();
  }

  T& ref(const std::string& name) {
    typename std::map<std::string, SmallStack<T> >::iterator iter =
        table.find(name);
    if (iter == table.end() || iter->second.empty()) {
      std::cerr << "Symbol '" << name << "' not found" << std::endl;
      assert(false);
    }
    return iter->second.top_ref();
  }

  bool contains(const std::string& name) const {
    typename std::map<std::string, SmallStack<T> >::const_iterator iter =
        table.find(name);
    return iter != table.end() && !iter->second.empty();
  }

  /// Add a new (name, value) pair to the current scope. Hide old
  /// values that have this name until we pop this name.
  void push(const std::string& name, const T& value) {
    table[name].push(value);
  }

  /// A name goes out of scope. Restore whatever its old value
  /// was (or remove it entirely if there was nothing else of the
  /// same name in an outer scope).
  void pop(const std::string& name) {
    typename std::map<std::string, SmallStack<T> >::iterator iter =
        table.find(name);
    assert(iter != table.end() && "Name not in symbol table");
    iter->second.pop();
    if (iter->second.empty()) {
      table.erase(iter);
    }
  }

  class const_iterator {
    typename std::map<std::string, SmallStack<T> >::const_iterator iter;

   public:
    explicit const_iterator(
        const typename std::map<std::string, SmallStack<T> >::const_iterator& i)
        : iter(i) {}

    const_iterator() {}

    bool operator!=(const const_iterator& other) { return iter != other.iter; }

    void operator++() { ++iter; }

    const std::string& name() { return iter->first; }

    const SmallStack<T>& stack() { return iter->second; }

    const T& value() { return iter->second.top(); }
  };

  const_iterator cbegin() const { return const_iterator(table.begin()); }

  const_iterator cend() const { return const_iterator(table.end()); }

  class iterator {
    typename std::map<std::string, SmallStack<T> >::iterator iter;

   public:
    explicit iterator(
        typename std::map<std::string, SmallStack<T> >::iterator i)
        : iter(i) {}

    iterator() {}

    bool operator!=(const iterator& other) { return iter != other.iter; }

    void operator++() { ++iter; }

    const std::string& name() { return iter->first; }

    SmallStack<T>& stack() { return iter->second; }

    T& value() { return iter->second.top_ref(); }
  };

  iterator begin() { return iterator(table.begin()); }

  iterator end() { return iterator(table.end()); }
};

template <typename T>
std::ostream& operator<<(std::ostream& stream, Scope<T>& s) {
  stream << "{\n";
  typename Scope<T>::const_iterator iter;
  for (iter = s.cbegin(); iter != s.cend(); ++iter) {
    stream << "  " << iter.name() << "\n";
  }
  stream << "}";
  return stream;
}

}  // namespace internal
}  // namespace jmlang

#endif  // JMLANG_IR_SCOPE_H
