#include "snackis/ctx.hpp"
#include "snackis/msg.hpp"
#include "snackis/snackis.hpp"

namespace snackis {
  Msg::Msg(Ctx &ctx): Rec(ctx) { }

  Msg::Msg(Ctx &ctx, const str &type, const str &to):
    Rec(ctx), type(type), proto_rev(PROTO_REV), to(to) { }

  Msg::Msg(const db::Table<Msg> &tbl, const db::Rec<Msg> &rec):
    Rec(dynamic_cast<Ctx &>(tbl.ctx)), id(false) {
    copy(tbl, *this, rec);
  }
}
