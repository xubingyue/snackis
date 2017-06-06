#ifndef SNACKIS_DB_HPP
#define SNACKIS_DB_HPP

#include "snackis/invite.hpp"
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
    db::Col<BasicSetting, str> setting_val;
    db::Table<BasicSetting> settings;
    
    db::Col<Invite, str> invite_to;
    db::Col<Invite, Time> invite_posted_at, invite_accepted_at, invite_rejected_at;
    db::Table<Invite> invites;
	    
    db::Col<Peer, UId> peer_id;
    db::Col<Peer, str> peer_name, peer_email;
    db::Col<Peer, crypt::PubKey> peer_crypt_key;
    db::Col<Peer, Time> peer_created_at;
    db::Table<Peer> peers, peer_emails;

    db::Col<Thread, UId> thread_id;
    db::Col<Thread, str> thread_name;
    db::Col<Thread, Time> thread_started_at, thread_posted_at;
    db::Col<Thread, db::Rec<Peer>> thread_started_by;
    db::Table<Thread> threads;
    
    db::Col<Msg, UId> msg_id;
    db::Table<Msg> msgs;

    db::Col<Msg, str> msg_type;
    db::Col<Msg, int64_t> msg_proto_rev;
    db::Col<Msg, str> msg_from, msg_to;
    db::Col<Msg, Time> msg_fetched_at;
    db::Col<Msg, str> msg_peer_name;
    db::Col<Msg, crypt::PubKey> msg_crypt_key;
    db::Col<Msg, UId> msg_invite_id;
    db::Table<Msg> inbox;

    db::Table<Msg> outbox;

    Db(db::Ctx &ctx);
  };
}

#endif
