#ifndef SNACKIS_TIME_TYPE_HPP
#define SNACKIS_TIME_TYPE_HPP

#include "snackis/core/time.hpp"
#include "snackis/core/type.hpp"

namespace snackis {
  struct TimeType: public Type<Time> {
    TimeType();
    Time read(std::istream &in) const override;
    void write(const Time &val, std::ostream &out) const override;
  };

  extern const TimeType time_type;
}

#endif
