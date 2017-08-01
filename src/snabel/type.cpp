#include "snabel/box.hpp"
#include "snabel/type.hpp"

namespace snabel {
  BasicType::BasicType(const str &n):
    name(n)
  { }

  BasicType::~BasicType()
  { }

  bool operator <(const BasicType &x, const BasicType &y) {
    return x.name < y.name;
  }

  bool isa(const Box &val, const BasicType &typ) {
    return &val.type == &typ;
  }

  Seq::Seq(BasicType &elt):
    BasicType(snackis::fmt("*%0", elt.name)),
    elem_type(elt)
  { }

  Type::Type(const str &n):
    BasicType(n),
    seq(*this)
  { }
}
