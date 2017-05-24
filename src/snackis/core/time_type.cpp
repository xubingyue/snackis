#include "snackis/core/time_type.hpp"

namespace snackis {
  const TimeType time_type;

  TimeType::TimeType(): Type<Time>("Time") { }

  Time TimeType::read(std::istream &in) const {
    int64_t ms;
    in >> ms;
    return Time(std::chrono::milliseconds(ms));
  }
  
  void TimeType::write(const Time &val, std::ostream &out) const {
    auto epoch = val.time_since_epoch();
    auto ms = std::chrono::duration_cast<std::chrono::milliseconds>(epoch);
    out << (int64_t)ms.count();
  }
}
