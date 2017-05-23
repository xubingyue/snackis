#include "snackis.hpp"

#include <sstream>

namespace snackis {
  const int VERSION[3] = {0, 5, 0};

  const int64_t PROTO_REV = 1;

  std::string version_string() {
    std::stringstream out;

    for (int i = 0; i < 3; i++) {
      if (i) { out << "."; }
      out << VERSION[i];
    }
    
    return out.str();
  }
}
