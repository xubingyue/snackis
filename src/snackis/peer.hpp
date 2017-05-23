#ifndef SNACKIS_PEER_HPP
#define SNACKIS_PEER_HPP

#include "snackis/core/uid.hpp"
#include "snackis/core/time.hpp"

namespace snackis {
  struct Peer {
    UId id;
    Time invited_at, accepted_at;
    
    Peer();
  };
}

#endif
