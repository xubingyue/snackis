#ifndef SNACKIS_CTX_HPP
#define SNACKIS_CTX_HPP

#include "snackis/msg.hpp"
#include "snackis/peer.hpp"
#include "snackis/core/time.hpp"
#include "snackis/core/uid.hpp"
#include "snackis/db/col.hpp"
#include "snackis/db/ctx.hpp"
#include "snackis/db/table.hpp"

namespace snackis {
  struct Ctx {
    db::Ctx &db_ctx;

    db::Col<Peer, UId> peer_id;
    db::Col<Peer, std::string> peer_name;
    db::Col<Peer, std::string> peer_email;
    db::Col<Peer, Time> peer_invited_at;
    db::Col<Peer, Time> peer_accepted_at;
    db::Table<Peer> peers, peer_names, peer_emails;
    
    db::Col<Msg, UId> msg_id;
    db::Table<Msg> msgs;

    db::Col<Msg, std::string> msg_type;
    db::Col<Msg, int64_t> msg_proto_rev;
    db::Col<Msg, std::string> msg_sent_by;
    db::Col<Msg, Time> msg_fetched_at;
    db::Table<Msg> inbox;

    db::Col<Msg, std::string> msg_sent_to;
    db::Table<Msg> outbox;

    Ctx(db::Ctx &db_ctx);
  };
}

#endif
