#include <boost/uuid/uuid_generators.hpp>
#include <boost/uuid/uuid_io.hpp>
#include "snackis/core/uid.hpp"

namespace snackis {
  UId uid() {
    return uuids::random_generator()();
  }
  
  UId parse_uid(const string &in) {
    return uuids::string_generator()(in);
  }

  string format_uid(const UId &in) {
    return uuids::to_string(in);
  }
}
