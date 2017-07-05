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
    copy(*this, rec);
  }

  Post::Post(const Msg &msg):
    IdRec(msg.ctx, *db::get(msg.post, msg.ctx.db.post_id))
  {
    db::copy(*this, msg.post);
    peer_ids.erase(whoami(ctx).id);
    peer_ids.insert(msg.from_id);
  }

  void copy(Post &dst, const Msg &src) {
    db::copy(dst, src.post);    
    dst.peer_ids.insert(src.from_id);    
    dst.peer_ids.erase(whoami(src.ctx).id);
  }

  opt<Post> find_post_id(Ctx &ctx, UId id) {
    db::Rec<Post> rec;
    set(rec, ctx.db.post_id, id);
    if (!load(ctx.db.posts, rec)) { return nullopt; }
    return Post(ctx, rec);
  }

  void set_feed(Post &pst, Feed &fd) {
    pst.feed_id = fd.id;
    pst.peer_ids = fd.peer_ids;
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
