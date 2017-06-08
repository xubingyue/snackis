#ifndef SNACKIS_POST_HPP
#define SNACKIS_POST_HPP

#include "snackis/rec.hpp"
#include "snackis/core/str.hpp"
#include "snackis/core/time.hpp"
#include "snackis/core/uid.hpp"

namespace snackis {
  struct Peer;
  struct Thread;
  
  struct Post: public Rec {
    UId id;
    db::Rec<Thread> thread;
    Time at;
    db::Rec<Peer> by;
    str body;
    
    Post(Ctx &ctx, const db::Rec<Thread> &thread);
    Post(const db::Table<Post> &tbl, const db::Rec<Post> &rec);   
  };
}

#endif
