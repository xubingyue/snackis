#ifndef SNACKIS_FEED_HPP
#define SNACKIS_FEED_HPP

#include "snackis/peer.hpp"
#include "snackis/core/str.hpp"
#include "snackis/core/time.hpp"
#include "snackis/core/uid.hpp"

namespace snackis {
  struct Post;
  
  struct Feed: Rec {   
    UId id;
    str name;
    Time at;
    UId by_id;
    
    Feed(Ctx &ctx);
    Feed(const db::Table<Feed> &tbl, const db::Rec<Feed> &rec);   
  };

  opt<Feed> find_feed_id(Ctx &ctx, UId id);
  Feed get_feed_id(Ctx &ctx, UId id);
  std::vector<Post> last_posts(const Feed &feed, size_t max);
}

#endif