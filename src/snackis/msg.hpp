#ifndef SNACKIS_MSG_HPP
#define SNACKIS_MSG_HPP

#include "snackis/id_rec.hpp"
#include "snackis/project.hpp"
#include "snackis/task.hpp"
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
    db::Rec<Feed> feed;
    db::Rec<Post> post;
    db::Rec<Project> project;
    db::Rec<Task> task;

    Msg(Ctx &ctx, const str &type);
    Msg(Ctx &ctx, const db::Rec<Msg> &src);
  };
  
  str encode(Msg &msg);
  bool decode(Msg &msg, const str &in);
  void receive(Msg &msg);
}

#endif
