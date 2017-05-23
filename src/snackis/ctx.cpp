#include "snackis/ctx.hpp"
#include "snackis/core/int64_type.hpp"
#include "snackis/core/string_type.hpp"
#include "snackis/core/time_type.hpp"
#include "snackis/core/uid_type.hpp"
#include "snackis/db/ctx.hpp"

namespace snackis {
  Ctx::Ctx(db::Ctx &db_ctx):
    db_ctx(db_ctx),

    peer_id("id", uid_type, &Peer::id),
    peer_name("name", string_type, &Peer::name),
    peer_email("email", string_type, &Peer::email),
    peer_invited_at("invited_at", time_type, &Peer::invited_at),
    peer_accepted_at("accepted_at", time_type, &Peer::accepted_at),
    peers(db_ctx, "peers", {&peer_id},
	  {&peer_name, &peer_email, &peer_invited_at, &peer_accepted_at}),

    peer_names(db_ctx, "peer_names", {&peer_name, &peer_id}, {}),
    peer_emails(db_ctx, "peer_emails", {&peer_email}, {&peer_id}),
    
    msg_id("id", uid_type, &Msg::id),
    msgs(db_ctx, "msgs", {&msg_id}, {}),
    
    msg_type("type", string_type, &Msg::type),
    msg_proto_rev("proto_rev", int64_type, &Msg::proto_rev),
    msg_sent_by("sent_by", string_type, &Msg::sent_by),
    msg_fetched_at("fetched_at", time_type, &Msg::fetched_at),
    msg_peer_name("peer_name", string_type, &Msg::peer_name),
    inbox(db_ctx, "inbox", {&msg_id},
	  {&msg_type, &msg_proto_rev, &msg_fetched_at, &msg_peer_name}),

    msg_sent_to("sent_to", string_type, &Msg::sent_to),
    outbox(db_ctx, "outbox", {&msg_id},
	   {&msg_type, &msg_proto_rev, &msg_sent_by, &msg_sent_to,
	       &msg_peer_name}) {
    peers.indexes.insert(&peer_names);
    peers.indexes.insert(&peer_emails);
  }
}
