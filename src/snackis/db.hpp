#ifndef SNACKIS_DB_HPP
#define SNACKIS_DB_HPP

#include "snackis/msg.hpp"
#include "snackis/peer.hpp"
#include "snackis/setting.hpp"
#include "snackis/thread.hpp"
#include "snackis/core/str.hpp"
#include "snackis/crypt/pub_key.hpp"
#include "snackis/db/col.hpp"
#include "snackis/db/ctx.hpp"
#include "snackis/db/table.hpp"

namespace snackis {
  struct Db {
    db::Col<BasicSetting, str> setting_key;
    db::Col<BasicSetting, Data> setting_val;
    db::Table<BasicSetting> settings;
    
    db::Col<Peer, UId> peer_id;
    db::Col<Peer, str> peer_name;
    db::Col<Peer, str> peer_email;
    db::Col<Peer, crypt::PubKey> peer_crypt_key;
    db::Col<Peer, Time> peer_invited_at;
    db::Col<Peer, Time> peer_accepted_at;
    db::Table<Peer> peers, peer_names, peer_emails;

    db::Col<Thread, UId> thread_id;
    db::Col<Thread, str> thread_name;
    db::Col<Thread, Time> thread_started_at;
    db::Col<Thread, Time> thread_posted_at;
    db::Col<Thread, Peer *> thread_started_by;
    db::Table<Thread> threads, thread_names;
    
    db::Col<Msg, UId> msg_id;
    db::Table<Msg> msgs;

    db::Col<Msg, str> msg_type;
    db::Col<Msg, int64_t> msg_proto_rev;
    db::Col<Msg, Time> msg_fetched_at;
    db::Col<Msg, str> msg_peer_name;
    db::Col<Msg, str> msg_peer_email;
    db::Col<Msg, crypt::PubKey> msg_crypt_key;
    db::Table<Msg> inbox;

    db::Col<Msg, Peer *> msg_sent_to;
    db::Table<Msg> outbox;

    Db(db::Ctx &ctx);
  };
}

#endif
