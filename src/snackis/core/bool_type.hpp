#ifndef SNACKIS_BOOL_TYPE_HPP
#define SNACKIS_BOOL_TYPE_HPP

#include <cstdint>
#include "snackis/core/type.hpp"

namespace snackis {
  struct BoolType: Type<bool> {
    BoolType();
    bool is_null(const bool &val) const override;
    bool from_val(const Val &in) const override;
    Val to_val(const bool &in) const override;
    bool read(std::istream &in) const override;
    void write(const bool &val, std::ostream &out) const override;
  };

  extern BoolType bool_type;
}

#endif
