#include "snackis/snackis.hpp"
#include "snackis/msg.hpp"

namespace snackis {
  Msg::Msg(Ctx &ctx, const std::string &type):
    Rec(ctx), id(uid()), type(type), proto_rev(PROTO_REV) { }

  Msg::Msg(Ctx &ctx): Rec(ctx) { }
}
