#ifndef SNACKIS_UID_TYPE_HPP
#define SNACKIS_UID_TYPE_HPP

#include "snackis/core/type.hpp"
#include "snackis/core/uid.hpp"

namespace snackis {
  struct UIdType: public Type<UId> {
    UIdType();
  };

  extern const UIdType uid_type;
}

#endif
