#ifndef SNACKIS_STRING_TYPE_HPP
#define SNACKIS_STRING_TYPE_HPP

#include <string>
#include "snackis/core/type.hpp"

namespace snackis {
  struct StringType: public Type<string> {
    StringType();
  };

  extern const StringType string_type;
}

#endif
