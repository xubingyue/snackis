#ifndef SNACKIS_MSG_HPP
#define SNACKIS_MSG_HPP

#include "snackis/rec.hpp"
#include "snackis/core/str.hpp"
#include "snackis/core/time.hpp"
#include "snackis/core/uid.hpp"
#include "snackis/crypt/pub_key.hpp"

namespace snackis {
  struct Msg: Rec {
    static const str INVITE, ACCEPT, REJECT, POST;
    
    UId id;
    str type;
    Time fetched_at;
    str from, to;
    UId from_id, to_id;
    str peer_name;
    crypt::PubKey crypt_key;
    UId feed_id;
    str feed_name, feed_info;
    UId post_id;
    Time post_at;
    str post_body;
    
    Msg(Ctx &ctx);
    Msg(Ctx &ctx, const str &type);
    Msg(Ctx &ctx, const db::Rec<Msg> &src);
  };
  
  str encode(const Msg &msg);
  bool decode(Msg &msg, const str &in);
}

#endif
