#ifndef SNACKIS_UID_HPP
#define SNACKIS_UID_HPP

#include <boost/uuid/uuid.hpp>
#include <boost/uuid/uuid_io.hpp>

#include "snackis/core/str.hpp"

namespace snackis {
  using UId = boost::uuids::uuid;

  UId uid();
  UId parse_uid(const str &in);
  str format_uid(const UId &in);  
}

#endif
