#ifndef SNACKIS_POST_HPP
#define SNACKIS_POST_HPP

#include "snackis/rec.hpp"
#include "snackis/core/str.hpp"
#include "snackis/core/time.hpp"
#include "snackis/core/uid.hpp"

namespace snackis {
  struct Peer;
  struct Feed;
  
  struct Post: Rec {
    UId id, feed_id;
    Time at;
    UId by_id;
    str body;

    Post(Feed &feed);
    Post(const Msg &msg);
    Post(Ctx &ctx, const db::Rec<Post> &rec);   
  };

  opt<Post> find_post_id(Ctx &ctx, UId id);
  void post_msgs(const Post &post);
}

#endif
