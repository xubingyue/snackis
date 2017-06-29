#ifndef SNACKIS_POST_HPP
#define SNACKIS_POST_HPP

#include "snackis/id_rec.hpp"
#include "snackis/core/str.hpp"
#include "snackis/core/time.hpp"
#include "snackis/core/uid.hpp"

namespace snackis {
  struct Peer;
  struct Feed;
  
  struct Post: IdRec {
    UId feed_id, owner_id;
    Time created_at, changed_at;
    str body;
    std::set<UId> peer_ids;

    Post(Ctx &ctx);
    Post(Ctx &ctx, const db::Rec<Post> &rec);   
    Post(const Msg &msg);
  };

  opt<Post> find_post_id(Ctx &ctx, UId id);
  void send(const Post &post);
  void set_feed(Post &pst, Feed &fed);
}

#endif
