#ifndef SNACKIS_DB_HPP
#define SNACKIS_DB_HPP

#include "snackis/invite.hpp"
#include "snackis/msg.hpp"
#include "snackis/peer.hpp"
#include "snackis/post.hpp"
#include "snackis/setting.hpp"
#include "snackis/thread.hpp"
#include "snackis/core/str.hpp"
#include "snackis/core/vector_type.hpp"
#include "snackis/crypt/pub_key.hpp"
#include "snackis/db/col.hpp"
#include "snackis/db/ctx.hpp"
#include "snackis/db/table.hpp"

namespace snackis {
  struct Db {
    db::Col<BasicSetting, str> setting_key;
    db::Col<BasicSetting, str> setting_val;
    db::Table<BasicSetting> settings;
    
    db::Col<Invite, str>  invite_to;
    db::Col<Invite, Time> invite_posted_at;
    db::Table<Invite> invites;
	    
    db::Col<Peer, UId>           peer_id;
    db::Col<Peer, str>           peer_name;
    db::Col<Peer, str>           peer_email;
    db::Col<Peer, crypt::PubKey> peer_crypt_key;
    db::Col<Peer, Time>          peer_created_at;
    db::Table<Peer> peers, peer_emails;

    VectorType<db::Rec<Peer>> peers_type;

    db::Col<Thread, UId>           thread_id;
    db::Col<Thread, str>           thread_subject;
    db::Col<Thread, Time>          thread_started_at;
    db::Col<Thread, db::Rec<Peer>> thread_started_by;
    db::Col<Thread, Peers>         thread_peers;
    db::Table<Thread> threads;

    db::Col<Post, UId>             post_id;
    db::Col<Post, db::Rec<Thread>> post_thread;
    db::Col<Post, Time>            post_at;
    db::Col<Post, db::Rec<Peer>>   post_by;
    db::Col<Post, str>             post_body;
    db::Table<Post> posts;

    db::Col<Msg, UId>  msg_id;
    db::Col<Msg, str>  msg_type;
    db::Col<Msg, str>  msg_from;
    db::Col<Msg, str>  msg_to;
    db::Col<Msg, Time> msg_fetched_at;
    db::Col<Msg, str>  msg_peer_name;
    db::Col<Msg, crypt::PubKey> msg_crypt_key;
    db::Col<Msg, UId>  msg_thread_id;
    db::Col<Msg, Time> msg_post_at;
    db::Col<Msg, str>  msg_post_body;
    db::Table<Msg> msgs, inbox, outbox;

    Db(db::Ctx &ctx);
  };
}

#endif
