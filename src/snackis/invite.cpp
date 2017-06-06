#include "snackis/ctx.hpp"
#include "snackis/invite.hpp"
#include "snackis/msg.hpp"

namespace snackis {
  Invite::Invite(Ctx &ctx, const str &to): Rec(ctx), to(to) { }

  Invite::Invite(const db::Table<Invite> &tbl, const db::Rec<Invite> &rec):
    Rec(dynamic_cast<Ctx &>(tbl.ctx)) {
    copy(tbl, *this, rec);
  }

  void post(Invite &inv) {
    inv.posted_at = now();
    inv.accepted_at = nulltime;
    inv.rejected_at = nulltime;
    upsert(inv.ctx.db.invites, inv);

    Msg msg(inv.ctx, "invite", inv.to);
    insert(inv.ctx.db.outbox, msg);
  }
}
