#include "snackis/ctx.hpp"
#include "snackis/feed.hpp"

namespace snackis {
  Feed::Feed(Ctx &ctx, UId id):
    IdRec(ctx, id),
    owner_id(whoami(ctx).id),
    created_at(now()),
    changed_at(created_at),
    active(true),
    visible(true)
  { }

  Feed::Feed(Ctx &ctx, const db::Rec<Feed> &src): IdRec(ctx, null_uid) {
    copy(*this, src);
  }

  Feed::Feed(const Msg &msg):
    IdRec(msg.ctx, *db::get(msg.feed, msg.ctx.db.feed_id))
  {
    db::copy(*this, msg.feed);    
    peer_ids.erase(whoami(ctx).id);
    peer_ids.insert(msg.from_id);
  }

  opt<Feed> find_feed_id(Ctx &ctx, UId id) {
    db::Rec<Feed> rec;
    set(rec, ctx.db.feed_id, id);
    if (!load(ctx.db.feeds, rec)) { return nullopt; }
    return Feed(ctx, rec);
  }

  Feed get_feed_id(Ctx &ctx, UId id) {
    auto found(find_feed_id(ctx, id));
    
    if (!found) {
      ERROR(Db, fmt("Feed id not found: %0", id));
    }

    return *found;
  }

  std::vector<const db::Rec<Post> *> last_posts(const Feed &feed,
						const Time &end,
						size_t max) {
    Ctx &ctx(feed.ctx);
    std::vector<const db::Rec<Post> *> out;
    if (ctx.db.feed_posts.recs.empty()) { return out; }

    db::Rec<Post> key;
    set(key, ctx.db.post_feed_id, feed.id);
    set(key, ctx.db.post_created_at, end);
    auto found(ctx.db.feed_posts.recs.lower_bound(key));
    if (found == ctx.db.feed_posts.recs.begin()) { return out; }
    found--;
    
    while (out.size() < max) {
      if (*db::get(*found, ctx.db.post_feed_id) != feed.id) { break; }
      const db::Rec<Post> &rec(db::get(ctx.db.posts, *found));
      out.push_back(&rec);
      if (found == ctx.db.feed_posts.recs.begin()) { break; }
      found--;
    }

    return out;
  }
}
