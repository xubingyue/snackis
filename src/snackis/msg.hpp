#ifndef SNACKIS_MSG_HPP
#define SNACKIS_MSG_HPP

#include "snackis/id_rec.hpp"
#include "snackis/project.hpp"
#include "snackis/script.hpp"
#include "snackis/task.hpp"
#include "snackis/core/str.hpp"
#include "snackis/core/time.hpp"
#include "snackis/core/uid.hpp"
#include "snackis/crypt/pub_key.hpp"

namespace snackis {
  struct Msg: IdRec {
    static const str INVITE, ACCEPT, SCRIPT, POST, TASK;
    
    str type;
    Time fetched_at;
    str from, to;
    UId from_id, to_id;
    str peer_name;
    crypt::PubKey crypt_key;
    db::Rec<Script> script;
    db::Rec<Feed> feed;
    db::Rec<Post> post;
    db::Rec<Project> project;
    db::Rec<Task> task;

    Msg(Ctx &ctx, const str &type);
    Msg(Ctx &ctx, const db::Rec<Msg> &src);
  };

  extern db::Col<Msg, UId>              msg_id;
  extern db::Col<Msg, str>              msg_type;
  extern db::Col<Msg, str>              msg_from, msg_to;
  extern db::Col<Msg, UId>              msg_from_id, msg_to_id;
  extern db::Col<Msg, Time>             msg_fetched_at;
  extern db::Col<Msg, str>              msg_peer_name;
  extern db::Col<Msg, crypt::PubKey>    msg_crypt_key;
  extern db::Col<Msg, db::Rec<Script>>  msg_script;
  extern db::Col<Msg, db::Rec<Feed>>    msg_feed;
  extern db::Col<Msg, db::Rec<Post>>    msg_post;
  extern db::Col<Msg, db::Rec<Project>> msg_project;
  extern db::Col<Msg, db::Rec<Task>>    msg_task;
  
  str encode(Msg &msg);
  bool decode(Msg &msg, const str &in);
  void receive(Msg &msg);
}

#endif
