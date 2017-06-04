#include "snackis/ctx.hpp"
#include "snackis/peer.hpp"

namespace snackis {
  Peer::Peer(Ctx &ctx): Rec(ctx) { }

  Peer::Peer(const db::Table<Peer> &tbl, const db::Rec<Peer> &rec):
    Rec(dynamic_cast<Ctx &>(tbl.ctx)), id(false) {
    copy(tbl, *this, rec);
  }
}
