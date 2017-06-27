#ifndef SNACKIS_TIME_HPP
#define SNACKIS_TIME_HPP

#include <chrono>
#include "snackis/core/str.hpp"

namespace snackis {
  using Clock = std::chrono::system_clock;
  using Time = std::chrono::time_point<Clock>;

  extern const Time null_time, max_time;
  
  Time now();
  str fmt(const Time &time, const str &spec);
  opt<Time> parse_time(const str &spec, const str &in, opt<Time> empty=nullopt);
};

#endif
