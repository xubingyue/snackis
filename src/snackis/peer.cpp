#include "snackis/ctx.hpp"
#include "snackis/peer.hpp"
#include "snackis/core/bool_type.hpp"
#include "snackis/core/time_type.hpp"
#include "snackis/core/uid_type.hpp"
#include "snackis/crypt/pub_key_type.hpp"

namespace snackis {
  db::Col<Peer, UId> peer_id("id", uid_type, &Peer::id);
  db::Col<Peer, Time> peer_created_at("created_at", time_type, &Peer::created_at);
  db::Col<Peer, Time> peer_changed_at("changed_at", time_type, &Peer::changed_at);
  db::Col<Peer, str>  peer_name("name", str_type, &Peer::name);
  db::Col<Peer, str>  peer_email("email", str_type, &Peer::email);
  db::Col<Peer, str>  peer_info("info", str_type, &Peer::info);
  db::Col<Peer, std::set<str>> peer_tags("tags", str_set_type, &Peer::tags);
  db::Col<Peer, crypt::PubKey> peer_crypt_key("crypt_key",
					      crypt::pub_key_type,
					      &Peer::crypt_key);
  db::Col<Peer, bool> peer_active("active", bool_type, &Peer::active);

  Peer::Peer(Ctx &ctx):
    IdRec(ctx),
    created_at(now()),
    changed_at(created_at),
    active(true)
  { }

  Peer::Peer(Ctx &ctx, const db::Rec<Peer> &src):
    IdRec(ctx, null_uid),
    active(true)
  {
    copy(*this, src);
  }
  
  Peer::Peer(const Msg &msg):
    IdRec(msg.ctx, msg.from_id)
  {
    Ctx &ctx(msg.ctx);

    if (!load(ctx.db.peers, *this)) {
      created_at = now();
      changed_at = created_at;
      name = msg.peer_name;
      email = msg.from;
      active = true;
    }
    
    crypt_key = msg.crypt_key;
  }

  opt<Peer> find_peer_id(Ctx &ctx, const UId &id) {
    db::Rec<Peer> rec;
    set(rec, peer_id, id);
    if (!load(ctx.db.peers, rec)) { return nullopt; }
    return Peer(ctx, rec);
  }

  Peer get_peer_id(Ctx &ctx, const UId &id) {
    auto found(find_peer_id(ctx, id));
    CHECK(found, _);
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
