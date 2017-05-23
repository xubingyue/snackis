#ifndef SNACKIS_MSG_HPP
#define SNACKIS_MSG_HPP

#include <string>

#include "snackis/peer.hpp"
#include "snackis/rec.hpp"
#include "snackis/core/time.hpp"
#include "snackis/core/uid.hpp"

namespace snackis {
  struct Msg: public Rec {
    UId id;
    std::string type;
    int64_t proto_rev;
    Time fetched_at;
    Peer *sent_to;
    std::string peer_name, peer_email;
    
    Msg(Ctx &ctx, const std::string &type);
    Msg(Ctx &ctx);
  };
}

#endif
