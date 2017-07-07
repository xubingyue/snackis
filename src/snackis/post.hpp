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
    std::set<str> tags;
    std::set<UId> peer_ids;
    
    Post(Ctx &ctx);
    Post(Ctx &ctx, const db::Rec<Post> &rec);   
    Post(const Msg &msg);
  };

  void copy(Post &dst, const Msg &src);
  opt<Post> find_post_id(Ctx &ctx, UId id);
  void send(const Post &post);
  Feed get_reply_feed(const Post &ps);
  void set_feed(Post &ps, Feed &fd);
}

#endif
