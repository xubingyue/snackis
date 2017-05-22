#include "snackis/core/path.hpp"

namespace snackis {
  bool create(const Path &path) {
    return boost::filesystem::create_directories(path);
  }
}
