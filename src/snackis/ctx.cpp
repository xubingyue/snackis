#include "snackis/ctx.hpp"
#include "snackis/core/data_type.hpp"
#include "snackis/core/int64_type.hpp"
#include "snackis/core/string_type.hpp"
#include "snackis/core/time_type.hpp"
#include "snackis/core/uid_type.hpp"
#include "snackis/crypt/pub_key_type.hpp"
#include "snackis/db/ctx.hpp"

namespace snackis {
  Ctx::Ctx(db::Ctx &db_ctx):
    db_ctx(db_ctx),

    setting_key("key", string_type, &Setting::key),
    setting_val("val", data_type, &Setting::val),
    settings(db_ctx, "settings", {&setting_key}, {&setting_val}),
    
    peer_id("id", uid_type, &Peer::id),
    peer_name("name", string_type, &Peer::name),
    peer_email("email", string_type, &Peer::email),
    peer_key("key", crypt::pub_key_type, &Peer::key),
    peer_invited_at("invited_at", time_type, &Peer::invited_at),
    peer_accepted_at("accepted_at", time_type, &Peer::accepted_at),
    peers(db_ctx, "peers", {&peer_id},
	  {&peer_name, &peer_email, &peer_invited_at, &peer_accepted_at}),

    peer_names(db_ctx, "peer_names", {&peer_name, &peer_id}, {}),
    peer_emails(db_ctx, "peer_emails", {&peer_email}, {&peer_id}),

    thread_id("id", uid_type, &Thread::id),
    thread_name("name", string_type, &Thread::name),
    thread_started_at("started_at", time_type, &Thread::started_at),
    thread_posted_at("posted_at", time_type, &Thread::posted_at),
    thread_started_by("started_by", peers.rec_type, &Thread::started_by),
    threads(db_ctx, "threads", {&thread_id},
	    {&thread_name, &thread_started_at, &thread_posted_at,
		&thread_started_by}),

    thread_names(db_ctx, "thread_names", {&thread_name, &thread_id}, {}),

    msg_id("id", uid_type, &Msg::id),
    msgs(db_ctx, "msgs", {&msg_id}, {}),
    
    msg_type("type", string_type, &Msg::type),
    msg_proto_rev("proto_rev", int64_type, &Msg::proto_rev),
    msg_fetched_at("fetched_at", time_type, &Msg::fetched_at),
    msg_peer_name("peer_name", string_type, &Msg::peer_name),
    msg_peer_email("peer_email", string_type, &Msg::peer_email),
    msg_peer_key("peer_key", crypt::pub_key_type, &Msg::peer_key),
    inbox(db_ctx, "inbox", {&msg_id},
	  {&msg_type, &msg_proto_rev, &msg_fetched_at, &msg_peer_name,
	      &msg_peer_email, &msg_peer_key}),

    msg_sent_to("sent_to", peers.rec_type, &Msg::sent_to),
    outbox(db_ctx, "outbox", {&msg_id},
	   {&msg_type, &msg_proto_rev, &msg_sent_to,
	       &msg_peer_name, &msg_peer_email, &msg_peer_key}) {
    peers.indexes.insert(&peer_names);
    peers.indexes.insert(&peer_emails);

    threads.indexes.insert(&thread_names);
  }
}
