#include "snackis/ctx.hpp"
#include "snackis/invite.hpp"
#include "snackis/msg.hpp"

namespace snackis {
  Invite::Invite(Ctx &ctx, const str &to): Rec(ctx), to(to) { }

  Invite::Invite(Ctx &ctx, const db::Rec<Invite> &src): Rec(ctx) {
    copy(*this, src);
  }

  void send(Invite &inv) {
    Ctx &ctx(inv.ctx);
    inv.posted_at = now();
    upsert(ctx.db.invites, inv);

    Msg msg(ctx, Msg::INVITE, inv.to);
    insert(inv.ctx.db.outbox, msg);
  }

  Peer accept_invite(const Msg &in) {
    Ctx &ctx(in.ctx);

    db::Rec<Peer> peer_rec;
    set(peer_rec, ctx.db.peer_email, in.from);
    if (load(ctx.db.peer_emails, peer_rec)) {
      load(ctx.db.peers, peer_rec);
    }
    
    Peer peer(ctx, peer_rec);
    peer.email = in.from;
    peer.name = in.peer_name;
    peer.crypt_key = in.crypt_key;
    upsert(ctx.db.peers, peer);

    Msg out(ctx, Msg::ACCEPT, in.from);
    insert(ctx.db.outbox, out);
    return peer;
  }

  Peer invite_accepted(const Msg &in) {
    Ctx &ctx(in.ctx);

    db::Rec<Invite> inv;
    set(inv, ctx.db.invite_to, in.from);
    erase(ctx.db.invites, inv);

    db::Rec<Peer> peer_rec;
    set(peer_rec, ctx.db.peer_email, in.from);
    if (load(ctx.db.peer_emails, peer_rec)) {
      load(ctx.db.peers, peer_rec);
    }
    
    Peer peer(ctx, peer_rec);
    peer.email = in.from;
    peer.name = in.peer_name;
    peer.crypt_key = in.crypt_key;
    upsert(ctx.db.peers, peer);
    return peer;
  }

  void reject_invite(const Msg &in) {
    Ctx &ctx(in.ctx);
    Msg out(ctx, Msg::REJECT, in.from, false);
    insert(ctx.db.outbox, out);
  }

  void invite_rejected(const Msg &in) {
    Ctx &ctx(in.ctx);
    db::Rec<Invite> inv;
    set(inv, ctx.db.invite_to, in.from);
    erase(ctx.db.invites, inv);
  }
}
