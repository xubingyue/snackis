#ifndef SNACKIS_THREAD_HPP
#define SNACKIS_THREAD_HPP

#include "snackis/rec.hpp"
#include "snackis/core/str.hpp"
#include "snackis/core/time.hpp"
#include "snackis/core/uid.hpp"

namespace snackis {
  struct Peer;
  
  struct Thread: public Rec {
    UId id;
    str name;
    Time started_at;
    db::Rec<Peer> started_by;
    
    Thread(Ctx &ctx);
    Thread(const db::Table<Thread> &tbl, const db::Rec<Thread> &rec);   
  };
}

#endif
