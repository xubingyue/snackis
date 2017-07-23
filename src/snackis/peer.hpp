#ifndef SNACKIS_PEER_HPP
#define SNACKIS_PEER_HPP

#include "snackis/id_rec.hpp"
#include "snackis/core/path.hpp"
#include "snackis/core/str.hpp"
#include "snackis/core/time.hpp"
#include "snackis/core/uid.hpp"
#include "snackis/crypt/pub_key.hpp"
#include "snackis/db/rec.hpp"

namespace snackis {
  struct Msg;
  
  struct Peer: IdRec {
    Time created_at, changed_at;
    str name, email, info;
    std::set<str> tags;
    crypt::PubKey crypt_key;
    bool active;
    
    Peer(Ctx &ctx);
    Peer(Ctx &ctx, const db::Rec<Peer> &src);
    Peer(const Msg &msg);
  };

  extern db::Col<Peer, UId>           peer_id;
  extern db::Col<Peer, Time>          peer_created_at, peer_changed_at;
  extern db::Col<Peer, str>           peer_name, peer_email, peer_info;
  extern db::Col<Peer, std::set<str>> peer_tags;
  extern db::Col<Peer, crypt::PubKey> peer_crypt_key;
  extern db::Col<Peer, bool>          peer_active;
  
  opt<Peer> find_peer_id(Ctx &ctx, const UId &id);
  Peer get_peer_id(Ctx &ctx, const UId &id);
  void encrypt(const Peer &peer, const Path &in, const Path &out, bool encode);
  void decrypt(const Peer &peer, const Path &in, const Path &out, bool encode);
}

#endif
