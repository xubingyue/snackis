#include "snackis/db.hpp"
#include "snackis/core/int64_type.hpp"
#include "snackis/core/str_type.hpp"
#include "snackis/core/time_type.hpp"
#include "snackis/core/uid_type.hpp"
#include "snackis/core/vector_type.hpp"
#include "snackis/crypt/pub_key_type.hpp"

namespace snackis {
  Db::Db(db::Ctx &ctx):
    setting_key("key", str_type, &BasicSetting::key),
    setting_val("val", str_type, &BasicSetting::val),
    
    settings(ctx, "settings",
	     {&setting_key}, {&setting_val}),

    invite_to(       "to",        str_type,  &Invite::to),
    invite_posted_at("posted_at", time_type, &Invite::posted_at),

    invites(ctx, "invites",
	    {&invite_to}, {&invite_posted_at}),
    
    peer_id(        "id",         uid_type,            &Peer::id),
    peer_name(      "name",       str_type,            &Peer::name),
    peer_email(     "email",      str_type,            &Peer::email),
    peer_crypt_key( "crypt_key",  crypt::pub_key_type, &Peer::crypt_key),
    peer_created_at("created_at", time_type,           &Peer::created_at),

    peers(ctx, "peers", {&peer_id},
	  {&peer_name, &peer_email, &peer_crypt_key, &peer_created_at}),

    peer_emails(ctx, "peer_emails", {&peer_email}, {&peer_id}),

    thread_id(        "id",         uid_type,        &Thread::id),
    thread_subject(   "subject",    str_type,        &Thread::subject),
    thread_started_at("started_at", time_type,       &Thread::started_at),
    thread_started_by("started_by", peers.rec_type,  &Thread::started_by),
    thread_peer_ids(  "peer_ids",   rec_vector_type, &Thread::peer_ids),
    
    threads(ctx, "threads", {&thread_id},
	    {&thread_subject, &thread_started_at, &thread_started_by, 
		&thread_peer_ids}),

    post_id(       "id",        uid_type,       &Post::id),
    post_thread_id("thread_id", uid_type,       &Post::thread_id),
    post_at(       "at",        time_type,      &Post::at),
    post_by(       "by",        peers.rec_type, &Post::by),
    post_body(     "body",      str_type,       &Post::body),

    posts(ctx, "posts", {&post_id},
	  {&post_id, &post_thread_id, &post_at, &post_by, &post_body}),

    thread_posts(ctx, "thread_posts", {&post_thread_id, &post_at, &post_id},
	  {}),
    
    msg_id(        "id",            uid_type,            &Msg::id),
    msg_type(      "type",          str_type,            &Msg::type),
    msg_from(      "from",          str_type,            &Msg::from),
    msg_to(        "to",            str_type,            &Msg::to),
    msg_fetched_at("fetched_at",    time_type,           &Msg::fetched_at),
    msg_peer_name( "peer_name",     str_type,            &Msg::peer_name),
    msg_crypt_key( "crypt_key",     crypt::pub_key_type, &Msg::crypt_key),
    msg_thread_id( "thread_id",     uid_type,            &Msg::thread_id),
    msg_post_at(   "post_at",       time_type,           &Msg::post_at),
    msg_post_body( "post_body",     str_type,            &Msg::post_body),
    msgs(ctx, "msgs", {&msg_id},
	 {}),
    
    inbox(ctx, "inbox", {&msg_id},
	  {&msg_type, &msg_fetched_at, &msg_peer_name,
	      &msg_from, &msg_crypt_key, &msg_thread_id, &msg_post_at,
	      &msg_post_body}),
    
    outbox(ctx, "outbox", {&msg_id},
	   {&msg_type, &msg_to, &msg_peer_name, &msg_crypt_key, &msg_thread_id,
	       &msg_post_at, &msg_post_body}) {
    peers.indexes.insert(&peer_emails);
    posts.indexes.insert(&thread_posts);
  }
}
