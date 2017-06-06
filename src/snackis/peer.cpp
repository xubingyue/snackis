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
      ERROR(db::Db, fmt("Peer email not found: %0", email));
    }

    Peer peer(ctx.db.peers, rec);

    if (!load(ctx.db.peers, peer)) {
      ERROR(db::Db, fmt("Peer not found: %0", peer.id));
    }

    return peer;
  }

  void encrypt(const Peer &peer, const Path &in, const Path &out, bool encode) {
    std::ifstream in_file;
    in_file.open(in, std::ios::in | std::ios::binary);
    in_file.seekg(0, std::ios::end);
    Data in_buf(in_file.tellg(), 0);
    in_file.seekg(0);
    in_file.read(reinterpret_cast<char *>(&in_buf[0]), in_buf.size());
    in_file.close();
    
    Data out_buf(crypt::encrypt(*get_val(peer.ctx.settings.crypt_key),
				peer.crypt_key,
				&in_buf[0], in_buf.size()));
    
    if (encode) {
      const str hex(bin_hex(&out_buf[0], out_buf.size()));
      out_buf.assign(hex.begin(), hex.end());
    }
	  
    std::ofstream out_file;
    out_file.open(out, std::ios::out | std::ios::trunc | std::ios::binary);
    out_file.write(reinterpret_cast<const char *>(&out_buf[0]), out_buf.size());
    out_file.close();

  }
}
