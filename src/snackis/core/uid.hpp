#ifndef SNACKIS_UID_HPP
#define SNACKIS_UID_HPP

#include <boost/uuid/uuid.hpp>

namespace snackis {
  using namespace boost;
  using namespace std;
  
  using UId = uuids::uuid;

  UId uid();
  UId parse_uid(const string &in);
  string format_uid(const UId &in);  
}

#endif
