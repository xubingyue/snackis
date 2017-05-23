#include "snackis/core/fmt.hpp"

namespace snackis {
  fmt::fmt(const std::string &spec): boost::format(spec) { }

  fmt::operator std::string() {
    return str();
  }

  fmt::operator const char *() {
    return str().c_str();
  }
}
