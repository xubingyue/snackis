#ifndef SNACKIS_UID_HPP
#define SNACKIS_UID_HPP

#include <uuid/uuid.h>
#include "snackis/core/fmt.hpp"
#include "snackis/core/str.hpp"

namespace snackis {
  struct UId {
    uuid_t val;

    UId(bool init=true);
  };
  
  bool operator==(const UId &x, const UId &y);
  bool operator!=(const UId &x, const UId &y);
  bool operator<(const UId &x, const UId &y);

  template <>
  str fmt_arg(const UId &arg);
  opt<UId> parse_uid(const str &in);
}

#endif
