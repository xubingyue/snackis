#ifndef SNACKIS_TYPE_HPP
#define SNACKIS_TYPE_HPP

#include <string>

namespace snackis {
  using namespace std;
  
  template <typename ValT>
  struct Type {
    const string name; 
    Type(const string &name);
  };

  template <typename ValT>
  Type<ValT>::Type(const string &name): name(name) {
  }
}

#endif
