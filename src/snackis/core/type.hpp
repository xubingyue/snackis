#ifndef SNACKIS_TYPE_HPP
#define SNACKIS_TYPE_HPP

#include <iostream>
#include <string>

namespace snackis {
  template <typename ValT>
  struct Type {
    const std::string name; 
    Type(const std::string &name);
    virtual ValT read(std::istream &in) const=0;
    virtual void write(const ValT &val, std::ostream &out) const=0;
  };

  template <typename ValT>
  Type<ValT>::Type(const std::string &name): name(name) { }
}

#endif
