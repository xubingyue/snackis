#include "snackis/core/path.hpp"

namespace snackis {  
  bool create_path(const Path &p) {
    return std::experimental::filesystem::create_directories(p);
  }

  bool path_exists(const Path &p) {
    return std::experimental::filesystem::exists(p);
  }

  bool remove_path(const Path &p) {
    std::error_code e;
    std::experimental::filesystem::remove_all(p, e);
    return e.value() == 0;
  }
}
