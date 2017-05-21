#include <boost/uuid/uuid_generators.hpp>
#include <boost/uuid/uuid_io.hpp>
#include "snackis/core/uid.hpp"

namespace snackis {
  UId uid() {
    return boost::uuids::random_generator()();
  }
  
  UId parse_uid(const std::string &in) {
    return boost::uuids::string_generator()(in);
  }

  std::string format_uid(const UId &in) {
    return boost::uuids::to_string(in);
  }
}
