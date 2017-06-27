#include <ctime>
#include <iomanip>
#include "snackis/core/stream.hpp"
#include "snackis/core/time.hpp"

namespace snackis {
  const Time null_time;

  Time now() {
    Time tim(Clock::now());
    return std::chrono::time_point_cast<std::chrono::milliseconds>(tim);
  }

  str fmt(const Time &tim, const str &spec) {
    Stream buf;

    if (tim != Time::max()) {
      const time_t t = Clock::to_time_t(tim);
      tm tm;
      localtime_r(&t, &tm);
      buf << std::put_time(localtime_r(&t, &tm), spec.c_str());
    }
    
    return buf.str();
  }

  opt<Time> parse_time(const str &spec, const str &in, opt<Time> empty) {
    if (in.empty()) { return empty; }
    InStream buf(in);
    tm tm;
    buf >> std::get_time(&tm, spec.c_str());
    if (buf.fail()) { return nullopt; }
    return Clock::from_time_t(mktime(&tm));
  }
}
