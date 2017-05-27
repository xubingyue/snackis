#include "snackis/core/time_type.hpp"

namespace snackis {
  const TimeType time_type;

  TimeType::TimeType(): Type<Time>("Time") { }

  Time TimeType::from_val(const Val &in) const { return get<Time>(in); }

  Val TimeType::to_val(const Time &in) const { return in; }

  Time TimeType::read(std::istream &in) const {
    int64_t ms = -1;
    in.read((char *)&ms, sizeof ms);
    return Time(std::chrono::milliseconds(ms));
  }
  
  void TimeType::write(const Time &val, std::ostream &out) const {
    auto epoch = val.time_since_epoch();
    auto ms = std::chrono::duration_cast<std::chrono::milliseconds>(epoch);
    const int64_t cnt = ms.count();
    out.write((char *)&cnt, sizeof cnt);
  }
}
