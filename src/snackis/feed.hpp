#ifndef SNACKIS_FEED_HPP
#define SNACKIS_FEED_HPP

#include <set>

#include "snackis/peer.hpp"
#include "snackis/core/str.hpp"
#include "snackis/core/uid.hpp"

namespace snackis {
  struct Msg;
  struct Post;
  
  struct Feed: IdRec {   
    UId owner_id;
    Time created_at, changed_at;
    str name, info;
    bool active, visible;
    std::set<UId> peer_ids;
    
    Feed(Ctx &ctx, UId id=UId(true));
    Feed(Ctx &ctx, const db::Rec<Feed> &rec);   
    Feed(const Msg &msg);
  };

  void copy(Feed &dst, const Msg &src);
  opt<Feed> find_feed_id(Ctx &ctx, UId id);
  Feed get_feed_id(Ctx &ctx, UId id);
  std::vector<const db::Rec<Post> *> last_posts(const Feed &feed,
						const Time &end,
						size_t max);
}

#endif
