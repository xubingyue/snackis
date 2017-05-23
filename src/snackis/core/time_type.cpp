#include "snackis/core/time_type.hpp"

namespace snackis {
  const TimeType time_type;

  TimeType::TimeType(): Type<Time>("time") {
  }
}
