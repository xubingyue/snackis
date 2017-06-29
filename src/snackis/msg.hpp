#ifndef SNACKIS_MSG_HPP
#define SNACKIS_MSG_HPP

#include "snackis/id_rec.hpp"
#include "snackis/core/str.hpp"
#include "snackis/core/time.hpp"
#include "snackis/core/uid.hpp"
#include "snackis/crypt/pub_key.hpp"

namespace snackis {
  struct Msg: IdRec {
    static const str INVITE, ACCEPT, REJECT, POST, TASK;
    
    str type;
    Time fetched_at;
    str from, to;
    UId from_id, to_id;
    str peer_name;
    crypt::PubKey crypt_key;
    UId feed_id;
    str feed_name, feed_info;
    bool feed_visible;
    UId post_id;
    Time post_at;
    str post_body;
    std::set<UId> peer_ids;

    Msg(Ctx &ctx, const str &type);
    Msg(Ctx &ctx, const db::Rec<Msg> &src);
  };
  
  str encode(const Msg &msg);
  bool decode(Msg &msg, const str &in);
}

#endif
