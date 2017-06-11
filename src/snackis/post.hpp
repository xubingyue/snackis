#ifndef SNACKIS_POST_HPP
#define SNACKIS_POST_HPP

#include <set>

#include "snackis/rec.hpp"
#include "snackis/core/str.hpp"
#include "snackis/core/time.hpp"
#include "snackis/core/uid.hpp"

namespace snackis {
  struct Peer;
  struct Thread;
  
  struct Post: public Rec {
    UId id, thread_id;
    Time at;
    db::Rec<Peer> by;
    str body;
    std::set<UId> peer_ids;

    Post(Thread &thread);
    Post(const Msg &msg);
    Post(const db::Table<Post> &tbl, const db::Rec<Post> &rec);   
  };

  void post_msgs(const Post &post);
}

#endif
