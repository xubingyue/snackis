#include "snackis/core/path.hpp"

namespace snackis {  
  bool create_path(const Path &path) {
    return fs::create_directories(path);
  }

  bool path_exists(const Path &path) {
    return fs::exists(path);
  }
}
