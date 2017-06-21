#include <iostream>
#include "snackis/ctx.hpp"
#include "snackis/peer.hpp"
#include "snackis/post.hpp"

namespace snackis {
  Post::Post(Feed &feed, Peer &by):
    Rec(feed.ctx), feed_id(feed.id), at(now()), by_id(by.id)
  { }

  Post::Post(const Msg &msg):
    Rec(msg.ctx),
    id(msg.post_id),
    feed_id(msg.feed_id), 
    at(msg.post_at), 
    by_id(get_peer_email(ctx, msg.from).id),
    body(msg.post_body)
  { }

  Post::Post(Ctx &ctx, const db::Rec<Post> &rec): Rec(ctx), id(false) {
    copy(*this, rec);
  }

  opt<Post> find_post_id(Ctx &ctx, UId id) {
    db::Rec<Post> rec;
    set(rec, ctx.db.post_id, id);
    if (!load(ctx.db.posts, rec)) { return nullopt; }
    return Post(ctx, rec);
  }

  void create_msgs(const Post &post) {
    Ctx &ctx(post.ctx);
    Feed feed(get_feed_id(ctx, post.feed_id));
    
    for (auto &pid: feed.peer_ids) {
      Peer peer(get_peer_id(ctx, pid));
      Msg msg(ctx, Msg::POST, peer.email);
      msg.feed_id = feed.id;
      msg.feed_name = feed.name;
      msg.post_id = post.id;
      msg.post_at = post.at;
      msg.post_body = post.body;
      insert(ctx.db.outbox, msg);
    }
  }
}
