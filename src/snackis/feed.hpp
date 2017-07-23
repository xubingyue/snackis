#ifndef SNACKIS_FEED_HPP
#define SNACKIS_FEED_HPP

#include <set>

#include "snackis/peer.hpp"
#include "snackis/core/str.hpp"
#include "snackis/core/uid.hpp"
#include "snackis/db/schema.hpp"
#include "snackis/db/rec_type.hpp"

namespace snackis {
  struct Msg;
  struct Post;
  
  struct Feed: IdRec {   
    UId owner_id;
    Time created_at, changed_at;
    str name, info;
    bool active, visible;
    std::set<str> tags;
    std::set<UId> peer_ids;
    
    Feed(Ctx &ctx, UId id=UId(true));
    Feed(Ctx &ctx, const db::Rec<Feed> &rec);   
    Feed(const Msg &msg);
  };

  extern db::Col<Feed, UId>           feed_id;
  extern db::Col<Feed, UId>           feed_owner_id;
  extern db::Col<Feed, Time>          feed_created_at, feed_changed_at;
  extern db::Col<Feed, str>           feed_name, feed_info;
  extern db::Col<Feed, std::set<str>> feed_tags;
  extern db::Col<Feed, bool>          feed_active, feed_visible;
  extern db::Col<Feed, std::set<UId>> feed_peer_ids;

  extern db::Schema<Feed> feed_key, feed_cols;
  extern db::RecType<Feed> feed_type;
  
  void copy(Feed &fd, const Msg &msg);
  opt<Feed> find_feed_id(Ctx &ctx, UId id);
  Feed get_feed_id(Ctx &ctx, UId id);
  std::vector<const db::Rec<Post> *> last_posts(const Feed &fd,
						const Time &end,
						size_t max);
}

#endif
