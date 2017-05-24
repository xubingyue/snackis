#ifndef SNACKIS_STRING_TYPE_HPP
#define SNACKIS_STRING_TYPE_HPP

#include <string>
#include "snackis/core/type.hpp"

namespace snackis {
  struct StringType: public Type<std::string> {
    StringType();
    std::string read(std::istream &in) const override;
    void write(const std::string &val, std::ostream &out) const override;
  };

  extern const StringType string_type;
}

#endif
