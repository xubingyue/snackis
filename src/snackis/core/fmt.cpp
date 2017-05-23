#include "snackis/core/fmt.hpp"

namespace snackis {
  Fmt fmt(const std::string spec) {
    return boost::format(spec);
  }
}
