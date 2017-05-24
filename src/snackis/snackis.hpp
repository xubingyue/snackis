#ifndef SNACKIS_HPP
#define SNACKIS_HPP

#include "snackis/core/str.hpp"

namespace snackis {
  extern const int VERSION[3];
  extern const int64_t PROTO_REV;
  
  void init();
  str version_str();
}

#endif
