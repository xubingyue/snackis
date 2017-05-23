#ifndef SNACKIS_CTX_HPP
#define SNACKIS_CTX_HPP

#include "snackis/msg.hpp"
#include "snackis/peer.hpp"
#include "snackis/core/uid.hpp"
#include "snackis/db/col.hpp"
#include "snackis/db/ctx.hpp"
#include "snackis/db/table.hpp"

namespace snackis {
  struct Ctx {
    db::Ctx &db_ctx;

    db::Col<Peer, UId> peer_id;
    db::Table<Peer> peers;

    db::Col<Msg, UId> msg_id;
    db::Table<Msg> msgs;

    Ctx(db::Ctx &db_ctx);
  };
}

#endif
