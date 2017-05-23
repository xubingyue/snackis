#ifndef SNACKIS_TIME_HPP
#define SNACKIS_TIME_HPP

#include <chrono>

namespace snackis {
  using Time = std::chrono::time_point<std::chrono::system_clock>;

  Time now();
};

#endif
