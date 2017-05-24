#ifndef SNACKIS_MSG_HPP
#define SNACKIS_MSG_HPP

#include "snackis/peer.hpp"
#include "snackis/rec.hpp"
#include "snackis/core/str.hpp"
#include "snackis/core/time.hpp"
#include "snackis/core/uid.hpp"
#include "snackis/crypt/pub_key.hpp"

namespace snackis {
  struct Msg: public Rec {
    UId id;
    str type;
    int64_t proto_rev;
    Time fetched_at;
    Peer *sent_to;
    str peer_name, peer_email;
    crypt::PubKey peer_key;
    
    Msg(Ctx &ctx);
    Msg(Ctx &ctx, const str &type);
    Msg(const db::Table<Msg> &tbl, const db::Rec<Msg> &rec);    
  };
}

#endif
