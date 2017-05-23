#include "snackis/snackis.hpp"
#include "snackis/msg.hpp"

namespace snackis {
  Msg::Msg(const std::string &type):
    id(uid()), type(type), proto_rev(PROTO_REV) { }

  Msg::Msg() { }
}
