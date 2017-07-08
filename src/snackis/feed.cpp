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
    db::copy(*this, src);
  }
  
  Feed::Feed(const Msg &msg):
    IdRec(msg.ctx, *db::get(msg.feed, msg.ctx.db.feed_id)),
    created_at(now()),
    changed_at(created_at)
  {
    copy(*this, msg);
  }

  void copy(Feed &dst, const Msg &src) {
    Ctx &ctx(src.ctx);
    Feed fd(ctx, src.feed);

    dst.id = fd.id;
    dst.owner_id = fd.owner_id;
    dst.name = fd.name;
    dst.info = fd.info;
    dst.active = fd.active;
    dst.visible = fd.visible;

    std::copy(fd.tags.begin(), fd.tags.end(),
	      std::inserter(dst.tags, dst.tags.end()));

    auto my_pid(whoami(ctx).id);
    std::copy_if(fd.peer_ids.begin(), fd.peer_ids.end(),
		 std::inserter(dst.peer_ids, dst.peer_ids.end()),
		 [&ctx, &my_pid](auto &pid) {
		   return find_peer_id(ctx, pid) && pid != my_pid;
		 });
    dst.peer_ids.insert(src.from_id);
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

  std::vector<const db::Rec<Post> *> last_posts(const Feed &fd,
						const Time &end,
						size_t max) {
    Ctx &ctx(fd.ctx);
    std::vector<const db::Rec<Post> *> out;
    if (ctx.db.feed_posts.recs.empty()) { return out; }

    db::Rec<Post> key;
    set(key, ctx.db.post_feed_id, fd.id);
    set(key, ctx.db.post_created_at, end);
    auto found(ctx.db.feed_posts.recs.lower_bound(key));
    if (found == ctx.db.feed_posts.recs.begin()) { return out; }
    found--;
    
    while (out.size() < max) {
      if (*db::get(*found, ctx.db.post_feed_id) != fd.id) { break; }
      const db::Rec<Post> &rec(db::get(ctx.db.posts, *found));
      out.push_back(&rec);
      if (found == ctx.db.feed_posts.recs.begin()) { break; }
      found--;
    }

    return out;
  }
}
