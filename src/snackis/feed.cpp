#include "snackis/ctx.hpp"
#include "snackis/feed.hpp"

namespace snackis {
  Feed::Feed(Ctx &ctx): Rec(ctx) { }

  Feed::Feed(const db::Table<Feed> &tbl, const db::Rec<Feed> &rec):
    Rec(dynamic_cast<Ctx &>(tbl.ctx)), id(false) {
    copy(tbl, *this, rec);
  }

  opt<Feed> find_feed_id(Ctx &ctx, UId id) {
    db::Rec<Feed> rec;
    set(rec, ctx.db.feed_id, id);
    if (!load(ctx.db.feeds, rec)) { return nullopt; }
    return Feed(ctx.db.feeds, rec);
  }

  Feed get_feed_id(Ctx &ctx, UId id) {
    auto found(find_feed_id(ctx, id));
    
    if (!found) {
      ERROR(Db, fmt("Feed id not found: %0", id));
    }

    return *found;
  }

  std::vector<Post> last_posts(const Feed &feed, size_t max) {
    Ctx &ctx(feed.ctx);
    db::Rec<Post> post_rec;
    set(post_rec, ctx.db.post_feed_id, feed.id);
    set(post_rec, ctx.db.post_at, Time::max());
    std::vector<Post> posts;
    if (ctx.db.feed_posts.recs.empty()) { return posts; }
    auto found(ctx.db.feed_posts.recs.lower_bound(post_rec));
    if (found == ctx.db.feed_posts.recs.begin()) { return posts; }
    found--;
    
    while (posts.size() < max) {
      post_rec.clear();
      copy(ctx.db.posts.key, post_rec, *found);
      load(ctx.db.posts, post_rec);
      Post post(ctx.db.posts, post_rec);
      if (post.feed_id != feed.id) { break; }
      posts.push_back(post);
      if (found == ctx.db.feed_posts.recs.begin()) { break; }
      found--;
    }

    return posts;
  }
}
