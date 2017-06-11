#ifndef SNACKIS_THREAD_HPP
#define SNACKIS_THREAD_HPP

#include "snackis/peer.hpp"
#include "snackis/core/str.hpp"
#include "snackis/core/time.hpp"
#include "snackis/core/uid.hpp"

namespace snackis {
  struct Thread: public Rec {   
    UId id;
    str subject;
    Time started_at;
    db::Rec<Peer> started_by;
    RecVector peer_ids;
    
    Thread(Ctx &ctx);
    Thread(const db::Table<Thread> &tbl, const db::Rec<Thread> &rec);   
  };
}

#endif
