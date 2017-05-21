#ifndef SNACKIS_INT64_TYPE_HPP
#define SNACKIS_INT64_TYPE_HPP

#include <cstdint>
#include "snackis/core/type.hpp"

namespace snackis {
  struct Int64Type: public Type<int64_t> {
    Int64Type();
  };

  extern const Int64Type int64_type;
}

#endif
