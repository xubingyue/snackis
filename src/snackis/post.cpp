#include <iostream>
#include "snackis/ctx.hpp"
#include "snackis/peer.hpp"
#include "snackis/post.hpp"

namespace snackis {
  Post::Post(Ctx &ctx): IdRec(ctx), at(now()), by_id(whoami(ctx).id)
  { }

  Post::Post(Ctx &ctx, const db::Rec<Post> &rec): IdRec(ctx, null_uid) {
    copy(*this, rec);
  }

  Post::Post(const Msg &msg):
    IdRec(msg.ctx, msg.post_id),
    feed_id(msg.feed_id), 
    at(msg.post_at), 
    by_id(msg.from_id),
    body(msg.post_body)
  { }

  opt<Post> find_post_id(Ctx &ctx, UId id) {
    db::Rec<Post> rec;
    set(rec, ctx.db.post_id, id);
    if (!load(ctx.db.posts, rec)) { return nullopt; }
    return Post(ctx, rec);
  }

  void send(const Post &post) {
    Ctx &ctx(post.ctx);
    Feed feed(get_feed_id(ctx, post.feed_id));
    
    for (auto &pid: feed.peer_ids) {
      Peer peer(get_peer_id(ctx, pid));
      Msg msg(ctx, Msg::POST);
      msg.to = peer.email;
      msg.to_id = peer.id;
      msg.feed_id = feed.id;
      msg.feed_name = feed.name;
      msg.feed_info = feed.info;
      msg.feed_visible = feed.visible;
      msg.post_id = post.id;
      msg.post_at = post.at;
      msg.post_body = post.body;
      insert(ctx.db.outbox, msg);
    }
  }
}
