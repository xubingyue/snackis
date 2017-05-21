#ifndef SNACKIS_TYPE_HPP
#define SNACKIS_TYPE_HPP

#include <string>

namespace snackis {
  template <typename ValT>
  struct Type {
    const std::string name; 
    Type(const std::string &name);
  };

  template <typename ValT>
  Type<ValT>::Type(const std::string &name): name(name) {
  }
}

#endif
