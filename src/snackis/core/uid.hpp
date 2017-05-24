#ifndef SNACKIS_UID_HPP
#define SNACKIS_UID_HPP

#include <boost/uuid/uuid.hpp>
#include <boost/uuid/uuid_io.hpp>
#include <string>

namespace snackis {
  using UId = boost::uuids::uuid;

  UId uid();
  UId parse_uid(const std::string &in);
  std::string format_uid(const UId &in);  
}

#endif
