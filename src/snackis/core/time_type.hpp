#ifndef SNACKIS_TIME_TYPE_HPP
#define SNACKIS_TIME_TYPE_HPP

#include "snackis/core/time.hpp"
#include "snackis/core/type.hpp"

namespace snackis {
  struct TimeType: Type<Time> {
    TimeType();
    Time from_val(const Val &in) const override;
    Val to_val(const Time &in) const override;

    Time read(std::istream &in) const override;
    void write(const Time &val, std::ostream &out) const override;
  };

  extern TimeType time_type;
}

#endif
