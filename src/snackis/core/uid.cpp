#include <boost/uuid/uuid_generators.hpp>
#include "snackis/core/uid.hpp"

namespace snackis {
  UId uid() {
    return boost::uuids::random_generator()();
  }
  
  UId parse_uid(const str &in) {
    return boost::uuids::string_generator()(in);
  }

  str format_uid(const UId &in) {
    return boost::uuids::to_string(in);
  }
}
