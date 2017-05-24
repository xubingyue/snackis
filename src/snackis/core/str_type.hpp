#ifndef SNACKIS_STR_TYPE_HPP
#define SNACKIS_STR_TYPE_HPP

#include "snackis/core/type.hpp"

namespace snackis {
  struct StrType: public Type<str> {
    StrType();
    str read(std::istream &in) const override;
    void write(const str &val, std::ostream &out) const override;
  };

  extern const StrType str_type;
}

#endif
