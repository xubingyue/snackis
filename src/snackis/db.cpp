#include "snackis/db.hpp"
#include "snackis/core/data_type.hpp"
#include "snackis/core/int64_type.hpp"
#include "snackis/core/str_type.hpp"
#include "snackis/core/time_type.hpp"
#include "snackis/core/uid_type.hpp"
#include "snackis/crypt/pub_key_type.hpp"

namespace snackis {
  Db::Db(db::Ctx &ctx):
    setting_key("key", str_type, &BasicSetting::key),
    setting_val("val", data_type, &BasicSetting::val),
    settings(ctx, "settings", {&setting_key}, {&setting_val}),
    
    peer_id("id", uid_type, &Peer::id),
    peer_name("name", str_type, &Peer::name),
    peer_email("email", str_type, &Peer::email),
    peer_crypt_key("crypt_key", crypt::pub_key_type, &Peer::crypt_key),
    peer_invited_at("invited_at", time_type, &Peer::invited_at),
    peer_accepted_at("accepted_at", time_type, &Peer::accepted_at),
    peers(ctx, "peers", {&peer_id},
	  {&peer_name, &peer_email, &peer_crypt_key, &peer_invited_at,
	      &peer_accepted_at}),

    peer_names(ctx, "peer_names", {&peer_name, &peer_id}, {}),
    peer_emails(ctx, "peer_emails", {&peer_email}, {&peer_id}),

    thread_id("id", uid_type, &Thread::id),
    thread_name("name", str_type, &Thread::name),
    thread_started_at("started_at", time_type, &Thread::started_at),
    thread_posted_at("posted_at", time_type, &Thread::posted_at),
    thread_started_by("started_by", peers.rec_type, &Thread::started_by),
    threads(ctx, "threads", {&thread_id},
	    {&thread_name, &thread_started_at, &thread_posted_at,
		&thread_started_by}),

    thread_names(ctx, "thread_names", {&thread_name, &thread_id}, {}),

    msg_id("id", uid_type, &Msg::id),
    msgs(ctx, "msgs", {&msg_id}, {}),
    
    msg_type("type", str_type, &Msg::type),
    msg_proto_rev("proto_rev", int64_type, &Msg::proto_rev),
    msg_fetched_at("fetched_at", time_type, &Msg::fetched_at),
    msg_peer_name("peer_name", str_type, &Msg::peer_name),
    msg_peer_email("peer_email", str_type, &Msg::peer_email),
    msg_crypt_key("peer_crypt_key", crypt::pub_key_type, &Msg::crypt_key),
    inbox(ctx, "inbox", {&msg_id},
	  {&msg_type, &msg_proto_rev, &msg_fetched_at, &msg_peer_name,
	      &msg_peer_email, &msg_crypt_key}),
    
    msg_sent_to("sent_to", peers.rec_type, &Msg::sent_to),
    outbox(ctx, "outbox", {&msg_id},
	   {&msg_type, &msg_proto_rev, &msg_sent_to,
	       &msg_peer_name, &msg_peer_email, &msg_crypt_key}) {
    peers.indexes.insert(&peer_names);
    peers.indexes.insert(&peer_emails);

    threads.indexes.insert(&thread_names);
  }
}
