#include "snackis/ctx.hpp"
#include "snackis/peer.hpp"

namespace snackis {
  Peer::Peer(Ctx &ctx): Rec(ctx), created_at(now()) { }

  Peer::Peer(const db::Table<Peer> &tbl, const db::Rec<Peer> &rec):
    Rec(dynamic_cast<Ctx &>(tbl.ctx)), id(false) {
    copy(tbl, *this, rec);
  }

  Peer get_email_peer(Ctx &ctx, const str &email) {
    db::Rec<Peer> rec;
    db::set(rec, ctx.db.peer_email, email);
    
    if (!load(ctx.db.peer_emails, rec)) {
      ERROR(Db, fmt("Peer email not found: %0", email));
    }

    Peer peer(ctx.db.peers, rec);

    if (!load(ctx.db.peers, peer)) {
      ERROR(Db, fmt("Peer not found: %0", peer.id));
    }

    return peer;
  }

  void encrypt(const Peer &peer, const Path &in, const Path &out, bool encode) {
    Data in_buf(slurp_data(in));
    Data out_buf(crypt::encrypt(*get_val(peer.ctx.settings.crypt_key),
				peer.crypt_key,
				&in_buf[0], in_buf.size()));
    
    if (encode) {
      const str hex(bin_hex(&out_buf[0], out_buf.size()));
      out_buf.assign(hex.begin(), hex.end());
    }

    dump_data(out_buf, out);
  }

  void decrypt(const Peer &peer, const Path &in, const Path &out, bool decode) {
    Data in_buf(slurp_data(in));

    if (decode) {
      const str hex(in_buf.begin(), in_buf.end());
      in_buf = hex_bin(hex);
    }

    Data out_buf(crypt::decrypt(*get_val(peer.ctx.settings.crypt_key),
				peer.crypt_key,
				&in_buf[0], in_buf.size()));
    
    dump_data(out_buf, out);
  }
}
