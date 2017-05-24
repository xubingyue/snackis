#ifndef SNACKIS_PEER_HPP
#define SNACKIS_PEER_HPP

#include "snackis/rec.hpp"
#include "snackis/core/uid.hpp"
#include "snackis/core/time.hpp"
#include "snackis/crypt/pub_key.hpp"
#include "snackis/db/rec.hpp"

namespace snackis {
  struct Peer: public Rec {
    UId id;
    std::string name, email;
    crypt::PubKey key;
    Time invited_at, accepted_at;
    
    Peer(Ctx &ctx);
    Peer(const db::Table<Peer> &tbl, const db::Rec<Peer> &rec);
  };
}

#endif
