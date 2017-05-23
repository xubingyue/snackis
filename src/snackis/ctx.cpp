#include "snackis/ctx.hpp"
#include "snackis/core/uid_type.hpp"
#include "snackis/db/ctx.hpp"

namespace snackis {
  Ctx::Ctx(db::Ctx &db_ctx):
    db_ctx(db_ctx),
    peer_id("id", uid_type, &Peer::id),
    peers(db_ctx, "peers", {&peer_id}, {}),
    msg_id("id", uid_type, &Msg::id),
    msgs(db_ctx, "msgs", {&msg_id}, {}) { }
}
