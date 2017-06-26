#include "snackis/ctx.hpp"
#include "snackis/feed.hpp"

namespace snackis {
  Feed::Feed(Ctx &ctx, UId id):
    Rec(ctx), id(id), owner_id(whoami(ctx).id), active(true)
  { }

  Feed::Feed(Ctx &ctx, const db::Rec<Feed> &src): Rec(ctx) {
    copy(*this, src);
  }

  Feed::Feed(const Msg &msg):
    Rec(msg.ctx), id(msg.feed_id), owner_id(msg.from_id), info(msg.feed_info) {
    db::Rec<Feed> rec;
    set(rec, ctx.db.feed_id, id);
    bool exists = load(ctx.db.feeds, rec) ? true : false;
    if (exists) {
      copy(*this, rec);
    }
    else {
      name = msg.feed_name;
    }
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
    set(key, ctx.db.post_at, end);
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
