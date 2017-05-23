#ifndef SNACKIS_THREAD_HPP
#define SNACKIS_THREAD_HPP

#include "snackis/rec.hpp"
#include "snackis/core/uid.hpp"
#include "snackis/core/time.hpp"

namespace snackis {
  struct Peer;
  
  struct Thread: public Rec {
    UId id;
    std::string name;
    Time started_at, posted_at;
    Peer *started_by;
    
    Thread(Ctx &ctx);
  };
}

#endif
