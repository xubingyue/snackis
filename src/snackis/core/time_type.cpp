#include "snackis/core/int64_type.hpp"
#include "snackis/core/time_type.hpp"

namespace snackis {
  TimeType time_type;

  TimeType::TimeType(): Type<Time>("Time") { }

  bool TimeType::is_null(const Time &val) const { return val == null_time; }

  Time TimeType::from_val(const Val &in) const { return get<Time>(in); }

  Val TimeType::to_val(const Time &in) const { return in; }

  Time TimeType::read(std::istream &in) const {
    int64_t ms(int64_type.read(in));
    return Time(std::chrono::milliseconds(ms));
  }
  
  void TimeType::write(const Time &val, std::ostream &out) const {
    auto epoch = val.time_since_epoch();
    auto ms = std::chrono::duration_cast<std::chrono::milliseconds>(epoch);
    int64_type.write(ms.count(), out);
  }
}
