#include "snackis/ctx.hpp"
#include "snackis/peer.hpp"

namespace snackis {
  Peer::Peer(Ctx &ctx): Rec(ctx) { }

  Peer::Peer(const db::Table<Peer> &tbl, const db::Rec<Peer> &rec):
    Rec(dynamic_cast<Ctx &>(tbl.ctx)), id(false) {
    copy(tbl, *this, rec);
  }

  Peer get_email_peer(Ctx &ctx, const str &email) {
    db::Rec<Peer> rec;
    db::set(rec, ctx.db.peer_email, email);
    
    if (!load(ctx.db.peer_emails, rec)) {
      ERROR(db::Db, fmt("Peer email not found: %0", email));
    }

    Peer peer(ctx.db.peers, rec);

    if (!load(ctx.db.peers, peer)) {
      ERROR(db::Db, fmt("Peer not found: %0", peer.id));
    }

    return peer;
  }
}
