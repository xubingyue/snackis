#include "snackis/ctx.hpp"
#include "snackis/feed.hpp"
#include "snackis/core/bool_type.hpp"
#include "snackis/core/time_type.hpp"
#include "snackis/core/uid_type.hpp"

namespace snackis {
  db::Col<Feed, UId> feed_id("id", uid_type, &Feed::id);
  db::Col<Feed, UId> feed_owner_id("owner_id", uid_type, &Feed::owner_id);
  db::Col<Feed, Time> feed_created_at("created_at", time_type, &Feed::created_at);
  db::Col<Feed, Time> feed_changed_at("changed_at", time_type, &Feed::changed_at);
  db::Col<Feed, str> feed_name("name", str_type, &Feed::name);
  db::Col<Feed, str> feed_info("into", str_type, &Feed::info);
  db::Col<Feed, std::set<str>> feed_tags("tags", str_set_type, &Feed::tags);
  db::Col<Feed, bool> feed_active("active", bool_type, &Feed::active);
  db::Col<Feed, bool> feed_visible("visible", bool_type, &Feed::visible);
  db::Col<Feed, std::set<UId>> feed_peer_ids("peer_ids",
					     uid_set_type,
					     &Feed::peer_ids);

  db::Key<Feed, UId> feed_key(feed_id);
  
  db::Schema<Feed> feed_cols({&feed_id, &feed_owner_id, &feed_created_at,
	&feed_changed_at, &feed_name, &feed_info, &feed_tags, &feed_active,
	&feed_visible, &feed_peer_ids});

  db::RecType<Feed> feed_type(feed_cols);

  Feed::Feed(Ctx &ctx, UId id):
    IdRec(ctx, id),
    owner_id(whoamid(ctx)),
    created_at(now()),
    changed_at(created_at),
    active(true),
    visible(true)
  { }

  Feed::Feed(Ctx &ctx, const db::Rec<Feed> &src): IdRec(ctx, null_uid) {
    db::copy(*this, src);
  }
  
  Feed::Feed(const Msg &msg):
    IdRec(msg.ctx, *db::get(msg.feed, feed_id)),
    owner_id(msg.from_id)
  {
    copy(*this, msg);
  }

  void copy(Feed &fd, const Msg &msg) {
    Ctx &ctx(fd.ctx);
    db::copy(ctx.db.feeds_share, fd, msg.feed);
    fd.peer_ids.erase(whoamid(ctx));
    fd.peer_ids.insert(msg.from_id);
  }

  opt<Feed> find_feed_id(Ctx &ctx, UId id) {
    db::Rec<Feed> rec;
    set(rec, feed_id, id);
    if (!load(ctx.db.feeds, rec)) { return nullopt; }
    return Feed(ctx, rec);
  }

  Feed get_feed_id(Ctx &ctx, UId id) {
    auto found(find_feed_id(ctx, id));
    CHECK(found, _);
    return *found;
  }

  std::vector<const db::Rec<Post> *> last_posts(const Feed &fd,
						const Time &end,
						size_t max) {
    Ctx &ctx(fd.ctx);
    std::vector<const db::Rec<Post> *> out;
    if (ctx.db.feed_posts.recs.empty()) { return out; }
    
    db::Rec<Post> key;
    set(key, post_feed_id, fd.id);
    set(key, post_created_at, end);
    auto found(ctx.db.feed_posts.recs.lower_bound(key));
    if (found == ctx.db.feed_posts.recs.begin()) { return out; }
    found--;
    
    while (out.size() < max) {
      if (*db::get(*found, post_feed_id) != fd.id) { break; }
      const db::Rec<Post> &rec(db::get(ctx.db.posts, *found));
      out.push_back(&rec);
      if (found == ctx.db.feed_posts.recs.begin()) { break; }
      found--;
    }

    return out;
  }
}
