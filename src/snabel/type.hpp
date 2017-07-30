#ifndef SNABEL_TYPE_HPP
#define SNABEL_TYPE_HPP

#include "snackis/core/str.hpp"

namespace snabel {  
  using namespace snackis;
  
  struct Type {
    const str name;
    Type(const str &n);
    Type(const Type &) = delete;
    const Type &operator =(const Type &) = delete;
  };
}

#endif
