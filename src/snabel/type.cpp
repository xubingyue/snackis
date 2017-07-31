#include "snabel/type.hpp"

namespace snabel {
  Type::Type(const str &n):
    name(n)
  { }

  bool operator <(const Type &x, const Type &y) {
    return x.name < y.name;
  }
}
