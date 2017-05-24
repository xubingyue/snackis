#ifndef SNACKIS_CTX_HPP
#define SNACKIS_CTX_HPP

#include "snackis/msg.hpp"
#include "snackis/peer.hpp"
#include "snackis/setting.hpp"
#include "snackis/thread.hpp"
#include "snackis/crypt/pub_key.hpp"
#include "snackis/db/col.hpp"
#include "snackis/db/ctx.hpp"
#include "snackis/db/table.hpp"

namespace snackis {
  struct Ctx {
    db::Ctx &db_ctx;

    db::Col<Setting, std::string> setting_key;
    db::Col<Setting, Data> setting_val;
    db::Table<Setting> settings;
    
    db::Col<Peer, UId> peer_id;
    db::Col<Peer, std::string> peer_name;
    db::Col<Peer, std::string> peer_email;
    db::Col<Peer, crypt::PubKey> peer_key;
    db::Col<Peer, Time> peer_invited_at;
    db::Col<Peer, Time> peer_accepted_at;
    db::Table<Peer> peers, peer_names, peer_emails;

    db::Col<Thread, UId> thread_id;
    db::Col<Thread, std::string> thread_name;
    db::Col<Thread, Time> thread_started_at;
    db::Col<Thread, Time> thread_posted_at;
    db::Col<Thread, void *> thread_started_by;
    db::Table<Thread> threads, thread_names;
    
    db::Col<Msg, UId> msg_id;
    db::Table<Msg> msgs;

    db::Col<Msg, std::string> msg_type;
    db::Col<Msg, int64_t> msg_proto_rev;
    db::Col<Msg, Time> msg_fetched_at;
    db::Col<Msg, std::string> msg_peer_name;
    db::Col<Msg, std::string> msg_peer_email;
    db::Col<Msg, crypt::PubKey> msg_peer_key;
    db::Table<Msg> inbox;

    db::Col<Msg, void *> msg_sent_to;
    db::Table<Msg> outbox;

    Ctx(db::Ctx &db_ctx);
  };
}

#endif
