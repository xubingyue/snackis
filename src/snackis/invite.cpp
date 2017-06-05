#include "snackis/ctx.hpp"
#include "snackis/invite.hpp"
#include "snackis/msg.hpp"

namespace snackis {
  Invite::Invite(Ctx &ctx, const str &to): Rec(ctx), to(to) { }

  Invite::Invite(const db::Table<Invite> &tbl, const db::Rec<Invite> &rec):
    Rec(dynamic_cast<Ctx &>(tbl.ctx)) {
    copy(tbl, *this, rec);
  }

  void send(Invite &inv) {
    inv.sent_at = now();
    inv.accept_at = nulltime;
    inv.reject_at = nulltime;
    upsert(inv.ctx.db.invites, inv);

    Msg msg(inv.ctx, "invite", inv.to);
    Peer &me(whoami(inv.ctx));
    msg.crypt_key = me.crypt_key;
    msg.peer_name = me.name;
    msg.peer_email = me.email;
    insert(inv.ctx.db.outbox, msg);
  }
}
