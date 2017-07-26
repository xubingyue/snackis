#include "snackis/ctx.hpp"
#include "snackis/invite.hpp"
#include "snackis/msg.hpp"
#include "snackis/core/time_type.hpp"

namespace snackis {
  db::Col<Invite, str>  invite_to("to", str_type, &Invite::to);
  db::Col<Invite, Time> invite_posted_at("posted_at", time_type, &Invite::posted_at);

  Invite::Invite(Ctx &ctx, const str &to): Rec(ctx), to(to)
  { }

  Invite::Invite(Ctx &ctx, const db::Rec<Invite> &src): Rec(ctx) {
    copy(*this, src);
  }

  void send(Invite &inv) {
    Ctx &ctx(inv.ctx);
    inv.posted_at = now();
    upsert(ctx.db.invites, inv);
    Msg msg(ctx, Msg::INVITE);
    msg.to = inv.to;
    insert(inv.ctx.db.outbox, msg);
  }
  
  void send_accept(const Msg &in) {
    Ctx &ctx(in.ctx);
    Peer pr(in);
    db::upsert(ctx.db.peers, pr);
    Msg out(ctx, Msg::ACCEPT);
    out.to = in.from;
    out.to_id = in.from_id;
    db::insert(ctx.db.outbox, out);
  }

  bool invite_accepted(const Msg &in) {
    Ctx &ctx(in.ctx);
    if (!db::erase(ctx.db.invites, in.from)) { return false; }
    
    Peer peer(in);
    db::upsert(ctx.db.peers, peer);
    return true;
  }
}
