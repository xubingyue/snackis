#ifndef SNABEL_TYPE_HPP
#define SNABEL_TYPE_HPP

#include "snackis/core/func.hpp"
#include "snackis/core/str.hpp"

namespace snabel {  
  using namespace snackis;

  struct Box;
  
  struct Type {
    const str name;
    
    func<str (const Box &)> fmt;
    
    Type(const str &n);
    Type(const Type &) = delete;
    const Type &operator =(const Type &) = delete;
  };

  bool operator <(const Type &x, const Type &y);
}

#endif
