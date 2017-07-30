#ifndef SNABEL_BOX_HPP
#define SNABEL_BOX_HPP

#include <variant>
#include <vector>
#include "snabel/type.hpp"

namespace snabel {  
  using namespace snackis;

  using Val = std::variant<int64_t, str>;
  
  struct Box {
    Type &type;
    Val val;
    
    Box(Type &t, const Val &v):
      type(t), val(v)
    { }
  };

  template <typename T>
  T get(const Val &v) { return std::get<T>(v); }
}

#endif
