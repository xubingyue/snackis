#include <iostream>
#include "snackis/ctx.hpp"
#include "snackis/peer.hpp"
#include "snackis/post.hpp"

namespace snackis {
  Post::Post(Ctx &ctx):
    IdRec(ctx),
    owner_id(whoami(ctx).id),
    created_at(now()),
    changed_at(created_at)
  { }

  Post::Post(Ctx &ctx, const db::Rec<Post> &rec): IdRec(ctx, null_uid) {
    db::copy(*this, rec);
    auto fd(find_feed_id(ctx, feed_id));
    if (fd) { peer_ids = fd->peer_ids; }
  }

  Post::Post(const Msg &msg):
    IdRec(msg.ctx, *db::get(msg.post, msg.ctx.db.post_id)),
    created_at(now()),
    changed_at(created_at)
  {
    copy(*this, msg);
  }

  void copy(Post &dst, const Msg &src) {
    Ctx &ctx(src.ctx);
    Post ps(get_post_id(ctx, *db::get(src.post, ctx.db.post_id)));

    dst.id = ps.id;
    dst.feed_id = ps.feed_id;
    dst.owner_id = ps.owner_id;
    dst.body = ps.body;
    dst.tags = ps.tags;

    auto my_pid(whoami(ctx).id);
    std::copy_if(ps.peer_ids.begin(), ps.peer_ids.end(),
		 std::inserter(dst.peer_ids, dst.peer_ids.end()),
		 [&ctx, &my_pid](auto &pid) {
		   return find_peer_id(ctx, pid) && pid != my_pid;
		 });
    dst.peer_ids.insert(src.from_id);    
  }

  opt<Post> find_post_id(Ctx &ctx, UId id) {
    db::Rec<Post> rec;
    set(rec, ctx.db.post_id, id);
    if (!load(ctx.db.posts, rec)) { return nullopt; }
    return Post(ctx, rec);
  }

  Post get_post_id(Ctx &ctx, UId id) {
    auto found(find_post_id(ctx, id));
    
    if (!found) {
      ERROR(Db, fmt("Post id not found: %0", id));
    }

    return *found;
  }

  Feed get_reply_feed(const Post &ps) {
    Ctx &ctx(ps.ctx);
    db::Rec<Feed> rec;
    db::set(rec, ctx.db.feed_id, ps.id);
    Feed fd(ctx, rec);

    if (!db::load(ctx.db.feeds, fd)) {
      db::Trans trans(ctx);
      TRY(try_create);
      fd.owner_id = ps.owner_id;
      fd.name = fmt("Post %0", id_str(ps));
      fd.tags = ps.tags;
      fd.active = true;
      fd.visible = false;
      fd.peer_ids = ps.peer_ids;
      db::insert(ctx.db.feeds, fd);
      if (try_create.errors.empty()) { db::commit(trans); }
    }
    
    return fd;
  }

  void set_feed(Post &ps, Feed &fd) {
    ps.feed_id = fd.id;
    ps.peer_ids = fd.peer_ids;
    std::copy(fd.tags.begin(), fd.tags.end(), std::inserter(ps.tags, ps.tags.end()));
  }

  void send(const Post &pst) {
    Ctx &ctx(pst.ctx);
    Feed fd(get_feed_id(ctx, pst.feed_id));
    
    for (auto &pid: pst.peer_ids) {
      auto pr(find_peer_id(ctx, pid));

      if (pr) {
	Msg msg(ctx, Msg::POST);
	msg.to = pr->email;
	msg.to_id = pr->id;
	db::copy(ctx.db.feeds, msg.feed, fd);
	db::copy(ctx.db.posts, msg.post, pst);
	insert(ctx.db.outbox, msg);
      }
    }
  }
}
