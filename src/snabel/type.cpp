#include "snabel/box.hpp"
#include "snabel/type.hpp"

namespace snabel {
  Type::Type(const str &n):
    name(n)
  { }

  Type::~Type()
  { }

  bool operator <(const Type &x, const Type &y) {
    return x.name < y.name;
  }

  bool isa(const Box &val, const Type &typ) {
    return &val.type == &typ;
  }
}
