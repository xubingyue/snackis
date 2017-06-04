#ifndef SNACKIS_UID_HPP
#define SNACKIS_UID_HPP

#include <uuid/uuid.h>
#include "snackis/core/str.hpp"

namespace snackis {
  struct UId {
    uuid_t val;

    UId(bool init=true);
  };
  
  bool operator<(const UId &x, const UId &y);
}

#endif
