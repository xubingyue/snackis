#include <iostream>
#include "snackis/ctx.hpp"
#include "snackis/peer.hpp"
#include "snackis/post.hpp"
#include "snackis/core/time_type.hpp"
#include "snackis/core/uid_type.hpp"

namespace snackis {
  db::Col<Post, UId> post_id("id", uid_type, &Post::id);
  db::Col<Post, UId> post_feed_id("feed_id", uid_type, &Post::feed_id);
  db::Col<Post, UId> post_owner_id("owner_id", uid_type, &Post::owner_id);
  db::Col<Post, Time> post_created_at("created_at", time_type, &Post::created_at);
  db::Col<Post, Time> post_changed_at("changed_at", time_type, &Post::changed_at);
  db::Col<Post, str> post_body("body", str_type, &Post::body);
  db::Col<Post, std::set<str>> post_tags("tags", str_set_type, &Post::tags);
  db::Col<Post, std::set<UId>> post_peer_ids("peer_ids",
					     uid_set_type,
					     &Post::peer_ids);

  db::Schema<Post> post_key({&post_id});
  db::Schema<Post> post_cols({&post_id, &post_feed_id, &post_owner_id, 
	&post_created_at, &post_changed_at, &post_body, &post_tags, &post_peer_ids});
  db::RecType<Post> post_type(post_cols);

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
    IdRec(msg.ctx, *db::get(msg.post, post_id)),
    owner_id(msg.from_id),
    created_at(now()),
    changed_at(created_at)
  {
    copy(*this, msg);
    peer_ids.insert(msg.from_id);
  }

  void copy(Post &ps, const Msg &msg) {
    Ctx &ctx(ps.ctx);
    db::copy(ctx.db.posts_share, ps, msg.post);
  }

  opt<Post> find_post_id(Ctx &ctx, UId id) {
    db::Rec<Post> rec;
    set(rec, post_id, id);
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
    db::set(rec, feed_id, ps.id);
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

  void send(const Post &ps, const Peer &pr) {
    Ctx &ctx(ps.ctx);
    Msg msg(ctx, Msg::POST);
    msg.to = pr.email;
    msg.to_id = pr.id;

    auto fd(get_feed_id(ctx, ps.feed_id));
    db::copy(ctx.db.feeds_share, msg.feed, fd);
    db::copy(ctx.db.posts_share, msg.post, ps);
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
