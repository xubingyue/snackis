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
  }

  Post::Post(const Msg &msg):
    IdRec(msg.ctx, *db::get(msg.post, msg.ctx.db.post_id)),
    owner_id(msg.from_id),
    created_at(now()),
    changed_at(created_at)
  {
    copy(*this, msg);
    peer_ids.insert(msg.from_id);
  }

  void copy(Post &ps, const Msg &msg) {
    Ctx &ctx(ps.ctx);
    ctx.db.post_id.copy(ps, msg.post);
    ctx.db.post_feed_id.copy(ps, msg.post);
    ctx.db.post_body.copy(ps, msg.post);
  }

  opt<Post> find_post_id(Ctx &ctx, UId id) {
    db::Rec<Post> rec;
    set(rec, ctx.db.post_id, id);
    if (!load(ctx.db.posts, rec)) { return nullopt; }
    return Post(ctx, rec);
  }

  Post get_post_id(Ctx &ctx, UId id) {
    auto found(find_post_id(ctx, id));
    CHECK(found, _);
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
      if (try_create.errors.empty()) {
	db::commit(trans, fmt("Created reply Feed %0", id_str(fd)));
      }
    }
    
    return fd;
  }

  void set_feed(Post &ps, Feed &fd) {
    Ctx &ctx(ps.ctx);
    
    if (ps.feed_id != null_uid) {
      Feed prev_fd(get_feed_id(ctx, ps.feed_id));
      for (auto &id: prev_fd.peer_ids) { ps.peer_ids.erase(id); }
      for (auto &t: prev_fd.tags) { ps.tags.erase(t); }
    }
    
    ps.feed_id = fd.id;
    for (auto &id: fd.peer_ids) { ps.peer_ids.insert(id); }
    for (auto &t: fd.tags) { ps.tags.insert(t); }
  }

  static void send(const Post &ps, const Peer &pr) {
    Ctx &ctx(ps.ctx);
    Msg msg(ctx, Msg::POST);
    msg.to = pr.email;
    msg.to_id = pr.id;

    auto fd(get_feed_id(ctx, ps.feed_id));
    ctx.db.feed_id.copy(msg.feed, fd);
    ctx.db.feed_name.copy(msg.feed, fd);
    ctx.db.feed_info.copy(msg.feed, fd);
    ctx.db.feed_active.copy(msg.feed, fd);
    ctx.db.feed_visible.copy(msg.feed, fd);
    
    ctx.db.post_id.copy(msg.post, ps);
    ctx.db.post_feed_id.copy(msg.post, ps);
    ctx.db.post_body.copy(msg.post, ps);
    insert(ctx.db.outbox, msg);
  }
  
  void send(const Post &ps) {
    Ctx &ctx(ps.ctx);

    for (auto &pid: ps.peer_ids) {
      auto pr(find_peer_id(ctx, pid));
      if (pr) { send(ps, *pr); }
    }
  }
}
