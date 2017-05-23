#ifndef SNACKIS_HPP
#define SNACKIS_HPP

#include <string>

namespace snackis {
  extern const int VERSION[3];
  extern const int64_t PROTO_REV;
  
  void init();
  std::string version_string();
}

#endif
