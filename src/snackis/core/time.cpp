#include <ctime>
#include <iomanip>
#include "snackis/core/buf.hpp"
#include "snackis/core/time.hpp"

namespace snackis {
  Time now() {
    Time tim(Clock::now());
    return std::chrono::time_point_cast<std::chrono::milliseconds>(tim);
  }

  str fmt_time(const Time &tim, const str &spec) {
    const time_t t = Clock::to_time_t(tim);
    tm tm;
    localtime_r(&t, &tm);
    Buf buf;
    buf << std::put_time(localtime_r(&t, &tm), spec.c_str());
    return buf.str();
  }
}
