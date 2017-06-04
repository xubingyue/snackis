#include "snackis/core/path.hpp"

namespace snackis {  
  bool create_path(const Path &p) {
    return std::experimental::filesystem::create_directories(p);
  }

  bool path_exists(const Path &p) {
    return std::experimental::filesystem::exists(p);
  }
}
