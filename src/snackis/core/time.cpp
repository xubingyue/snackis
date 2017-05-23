#include "snackis/core/time.hpp"

namespace snackis {
  Time now() {
    return std::chrono::system_clock::now();
  }
}
