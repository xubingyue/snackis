#ifndef SNACKIS_MSG_HPP
#define SNACKIS_MSG_HPP

#include "snackis/rec.hpp"
#include "snackis/core/str.hpp"
#include "snackis/core/time.hpp"
#include "snackis/core/uid.hpp"
#include "snackis/crypt/pub_key.hpp"

namespace snackis {
  struct Msg: public Rec {
    static const str INVITE, ACCEPT, REJECT, POST;
    
    UId id;
    str type;
    Time fetched_at;
    str from, to;
    str peer_name;
    crypt::PubKey crypt_key;
    UId thread_id;
    str post_body;
    Time post_at;
    
    Msg(Ctx &ctx);
    Msg(Ctx &ctx, const str &type, const str &to, bool init=true);
    Msg(const db::Table<Msg> &tbl, const db::Rec<Msg> &rec);
  };
  
  str encode(const Msg &msg);
  bool decode(Msg &msg, const str &in);
}

#endif
