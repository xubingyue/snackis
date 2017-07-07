#ifndef SNACKIS_STR_TYPE_HPP
#define SNACKIS_STR_TYPE_HPP

#include "snackis/core/set_type.hpp"
#include "snackis/core/type.hpp"

namespace snackis {
  struct StrType: Type<str> {
    StrType();
    bool is_null(const str &val) const override;
    str from_val(const Val &in) const override;
    Val to_val(const str &in) const override;
    str read(std::istream &in) const override;
    void write(const str &val, std::ostream &out) const override;
  };

  extern StrType str_type;
  extern SetType<str> str_set_type;
}

#endif
