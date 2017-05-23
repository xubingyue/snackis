#include "snackis/peer.hpp"

namespace snackis {
  Peer::Peer(Ctx &ctx): Rec(ctx), id(uid()) { }
}
