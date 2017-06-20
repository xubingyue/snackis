#ifndef SNACKIS_PEER_HPP
#define SNACKIS_PEER_HPP

#include "snackis/rec.hpp"
#include "snackis/core/path.hpp"
#include "snackis/core/str.hpp"
#include "snackis/core/time.hpp"
#include "snackis/core/uid.hpp"
#include "snackis/crypt/pub_key.hpp"
#include "snackis/db/rec.hpp"

namespace snackis {
  struct Peer: Rec {
    UId id;
    str name, email;
    crypt::PubKey crypt_key;
    
    Peer(Ctx &ctx);
    Peer(const db::Table<Peer> &tbl, const db::Rec<Peer> &rec);
  };

  Peer get_peer_id(Ctx &ctx, const UId &id);
  opt<Peer> find_peer_email(Ctx &ctx, const str &email);
  Peer get_peer_email(Ctx &ctx, const str &email);
  void encrypt(const Peer &peer, const Path &in, const Path &out, bool encode);
  void decrypt(const Peer &peer, const Path &in, const Path &out, bool encode);
}

#endif
