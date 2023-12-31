#include "jmlang/Lang/RDom.h"

#include "jmlang/Base/Util.h"
#include "jmlang/IR/IROperator.h"
#include "jmlang/IR/IRPrinter.h"
#include "jmlang/IR/Scope.h"

namespace jmlang {

using std::string;
using std::vector;

RVar::operator Expr() const {
  return internal::Variable::make(Int(32), name(), domain);
}

internal::ReductionDomain build_domain(string name0, Expr min0, Expr extent0,
                                       string name1, Expr min1, Expr extent1,
                                       string name2, Expr min2, Expr extent2,
                                       string name3, Expr min3, Expr extent3) {
  vector<internal::ReductionVariable> d;
  if (min0.defined()) {
    internal::ReductionVariable v = {name0, min0, extent0};
    d.push_back(v);
  }
  if (min1.defined()) {
    internal::ReductionVariable v = {name1, min1, extent1};
    d.push_back(v);
  }
  if (min2.defined()) {
    internal::ReductionVariable v = {name2, min2, extent2};
    d.push_back(v);
  }
  if (min3.defined()) {
    internal::ReductionVariable v = {name3, min3, extent3};
    d.push_back(v);
  }

  internal::ReductionDomain dom(d);

  return dom;
}

RDom::RDom(internal::ReductionDomain d) : domain(d) {
  const std::vector<internal::ReductionVariable>& vars = domain.domain();
  if (vars.size() > 0) {
    x = RVar(vars[0].var, vars[0].min, vars[0].extent, d);
  }
  if (vars.size() > 1) {
    y = RVar(vars[1].var, vars[1].min, vars[1].extent, d);
  }
  if (vars.size() > 2) {
    z = RVar(vars[2].var, vars[2].min, vars[2].extent, d);
  }
  if (vars.size() > 3) {
    w = RVar(vars[3].var, vars[3].min, vars[3].extent, d);
  }
}

// We suffix all RVars with $r to prevent unintentional name matches with pure
// vars called x, y, z, w.
RDom::RDom(Expr min, Expr extent, string name) {
  min = cast<int>(min);
  extent = cast<int>(extent);
  if (name == "")
    name = internal::unique_name('r');
  domain = build_domain(name + ".x$r", min, extent, "", Expr(), Expr(), "",
                        Expr(), Expr(), "", Expr(), Expr());
  x = RVar(name + ".x$r", min, extent, domain);
}

RDom::RDom(Expr min0, Expr extent0, Expr min1, Expr extent1, string name) {
  min0 = cast<int>(min0);
  extent0 = cast<int>(extent0);
  min1 = cast<int>(min1);
  extent1 = cast<int>(extent1);
  if (name == "")
    name = internal::unique_name('r');
  domain = build_domain(name + ".x$r", min0, extent0, name + ".y$r", min1,
                        extent1, "", Expr(), Expr(), "", Expr(), Expr());
  x = RVar(name + ".x$r", min0, extent0, domain);
  y = RVar(name + ".y$r", min1, extent1, domain);
}

RDom::RDom(Expr min0, Expr extent0, Expr min1, Expr extent1, Expr min2,
           Expr extent2, string name) {
  min0 = cast<int>(min0);
  extent0 = cast<int>(extent0);
  min1 = cast<int>(min1);
  extent1 = cast<int>(extent1);
  min2 = cast<int>(min2);
  extent2 = cast<int>(extent2);
  if (name == "")
    name = internal::unique_name('r');
  domain =
      build_domain(name + ".x$r", min0, extent0, name + ".y$r", min1, extent1,
                   name + ".z$r", min2, extent2, "", Expr(), Expr());
  x = RVar(name + ".x$r", min0, extent0, domain);
  y = RVar(name + ".y$r", min1, extent1, domain);
  z = RVar(name + ".z$r", min2, extent2, domain);
}

RDom::RDom(Expr min0, Expr extent0, Expr min1, Expr extent1, Expr min2,
           Expr extent2, Expr min3, Expr extent3, string name) {
  min0 = cast<int>(min0);
  extent0 = cast<int>(extent0);
  min1 = cast<int>(min1);
  extent1 = cast<int>(extent1);
  min2 = cast<int>(min2);
  extent2 = cast<int>(extent2);
  min3 = cast<int>(min3);
  extent3 = cast<int>(extent3);
  if (name == "")
    name = internal::unique_name('r');
  domain =
      build_domain(name + ".x$r", min0, extent0, name + ".y$r", min1, extent1,
                   name + ".z$r", min2, extent2, name + ".w$r", min3, extent3);
  x = RVar(name + ".x$r", min0, extent0, domain);
  y = RVar(name + ".y$r", min1, extent1, domain);
  z = RVar(name + ".z$r", min2, extent2, domain);
  w = RVar(name + ".w$r", min3, extent3, domain);
}

RDom::RDom(Buffer b) {
  Expr min[4], extent[4];
  for (int i = 0; i < 4; i++) {
    if (b.dimensions() > i) {
      min[i] = b.min(i);
      extent[i] = b.extent(i);
    }
  }
  string names[] = {b.name() + ".x$r", b.name() + ".y$r", b.name() + ".z$r",
                    b.name() + ".w$r"};
  domain =
      build_domain(names[0], min[0], extent[0], names[1], min[1], extent[1],
                   names[2], min[2], extent[2], names[3], min[3], extent[3]);
  RVar* vars[] = {&x, &y, &z, &w};
  for (int i = 0; i < 4; i++) {
    if (b.dimensions() > i) {
      *(vars[i]) = RVar(names[i], min[i], extent[i], domain);
    }
  }
}

RDom::RDom(ImageParam p) {
  Expr min[4], extent[4];
  for (int i = 0; i < 4; i++) {
    if (p.dimensions() > i) {
      min[i] = 0;
      extent[i] = p.extent(i);
    }
  }
  string names[] = {p.name() + ".x$r", p.name() + ".y$r", p.name() + ".z$r",
                    p.name() + ".w$r"};
  domain =
      build_domain(names[0], min[0], extent[0], names[1], min[1], extent[1],
                   names[2], min[2], extent[2], names[3], min[3], extent[3]);
  RVar* vars[] = {&x, &y, &z, &w};
  for (int i = 0; i < 4; i++) {
    if (p.dimensions() > i) {
      *(vars[i]) = RVar(names[i], min[i], extent[i], domain);
    }
  }
}

int RDom::dimensions() const { return (int)domain.domain().size(); }

RVar RDom::operator[](int i) {
  if (i == 0)
    return x;
  if (i == 1)
    return y;
  if (i == 2)
    return z;
  if (i == 3)
    return w;
  assert(false && "Reduction domain index out of bounds");
  return x;  // Keep the compiler happy
}

RDom::operator Expr() const {
  if (dimensions() != 1) {
    std::cerr << "Error: Can't treat this multidimensional RDom as an Expr:\n"
              << (*this) << "\n"
              << "Only single-dimensional RDoms can be cast to Expr.\n";
    assert(false);
  }
  return Expr(x);
}

RDom::operator RVar() const {
  if (dimensions() != 1) {
    std::cerr << "Error: Can't treat this multidimensional RDom as an RVar:\n"
              << (*this) << "\n"
              << "Only single-dimensional RDoms can be cast to RVar.\n";
    assert(false);
  }
  return x;
}

/** Emit an RVar in a human-readable form */
std::ostream& operator<<(std::ostream& stream, RVar v) {
  stream << v.name() << "(" << v.min() << ", " << v.extent() << ")";
  return stream;
}

/** Emit an RDom in a human-readable form. */
std::ostream& operator<<(std::ostream& stream, RDom dom) {
  stream << "RDom(\n";
  for (int i = 0; i < dom.dimensions(); i++) {
    stream << "  " << dom[i] << "\n";
  }
  stream << ")\n";
  return stream;
}

}  // namespace jmlang
