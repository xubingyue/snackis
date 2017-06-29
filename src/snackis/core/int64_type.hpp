#ifndef SNACKIS_INT64_TYPE_HPP
#define SNACKIS_INT64_TYPE_HPP

#include <cstdint>
#include "snackis/core/type.hpp"

namespace snackis {
  struct Int64Type: Type<int64_t> {
    Int64Type();
    bool is_null(const int64_t &val) const override;
    int64_t from_val(const Val &in) const override;
    Val to_val(const int64_t &in) const override;
    int64_t read(std::istream &in) const override;
    void write(const int64_t &val, std::ostream &out) const override;
  };

  extern Int64Type int64_type;
}

#endif
