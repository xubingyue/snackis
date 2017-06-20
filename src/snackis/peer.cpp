#include "snackis/ctx.hpp"
#include "snackis/peer.hpp"

namespace snackis {
  Peer::Peer(Ctx &ctx): Rec(ctx) { }

  Peer::Peer(Ctx &ctx, const db::Rec<Peer> &src): Rec(ctx), id(false) {
    copy(*this, src);
  }

  Peer get_peer_id(Ctx &ctx, const UId &id) {
    db::Rec<Peer> rec;
    set(rec, ctx.db.peer_id, id);

    if (!load(ctx.db.peers, rec)) {
      ERROR(Db, fmt("Peer id not found: %0", id));
    }

    return Peer(ctx, rec);
  }

  opt<Peer> find_peer_email(Ctx &ctx, const str &email) {
    db::Rec<Peer> rec;
    db::set(rec, ctx.db.peer_email, email);
    
    if (!load(ctx.db.peer_emails, rec)) { return nullopt; }

    Peer peer(ctx, rec);

    if (!load(ctx.db.peers, peer)) {
      ERROR(Db, fmt("Peer not found: %0", peer.id));
    }

    return peer;
  }

  Peer get_peer_email(Ctx &ctx, const str &email) {
    auto found(find_peer_email(ctx, email));
    
    if (!found) {
      ERROR(Db, fmt("Peer email not found: %0", email));
    }

    return *found;
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
