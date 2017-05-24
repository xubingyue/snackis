#include "snackis/core/fmt.hpp"

namespace snackis {
  fmt::fmt(const snackis::str &spec): boost::format(spec) { }

  fmt::operator snackis::str() const {
    return str();
  }

  fmt::operator const char *() const {
    return str().c_str();
  }
}
