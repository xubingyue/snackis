#include "snackis/core/time.hpp"

namespace snackis {
  Time now() {
    Time now(std::chrono::system_clock::now());
    return std::chrono::time_point_cast<std::chrono::milliseconds>(now);
  }
}
