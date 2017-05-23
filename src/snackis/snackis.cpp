#include "snackis.hpp"

#include <sstream>

namespace snackis {
  const int version[3] = {0, 5, 0};

  std::string version_string() {
    std::stringstream out;

    for (int i = 0; i < 3; i++) {
      if (i) { out << "."; }
      out << version[i];
    }
    
    return out.str();
  }
}
