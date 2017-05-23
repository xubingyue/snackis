#ifndef SNACKIS_PEER_HPP
#define SNACKIS_PEER_HPP

#include "snackis/rec.hpp"
#include "snackis/core/uid.hpp"
#include "snackis/core/time.hpp"

namespace snackis {
  struct Peer: public Rec {
    UId id;
    Time invited_at, accepted_at;
    
    Peer(Ctx &ctx);
  };
}

#endif
