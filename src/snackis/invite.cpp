#include "snackis/ctx.hpp"
#include "snackis/invite.hpp"

namespace snackis {
  Invite::Invite(Ctx &ctx, const str &to): Rec(ctx), to(to) { }

  Invite::Invite(const db::Table<Invite> &tbl, const db::Rec<Invite> &rec):
    Rec(dynamic_cast<Ctx &>(tbl.ctx)) {
    copy(tbl, *this, rec);
  }
}
