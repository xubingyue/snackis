#include "snackis/ctx.hpp"
#include "snackis/post.hpp"

namespace snackis {
  Post::Post(Ctx &ctx, const db::Rec<Thread> &thread):
    Rec(ctx), thread_id(*get(thread, ctx.db.thread_id))
  { }

  Post::Post(const db::Table<Post> &tbl, const db::Rec<Post> &rec):
    Rec(dynamic_cast<Ctx &>(tbl.ctx)), id(false) {
    copy(tbl, *this, rec);
  }
  
  void post_msgs(Post &post) {
    Ctx &ctx(post.ctx);
    Thread thread(get_thread_id(ctx, post.thread_id));
    
    for (auto &pid: thread.peer_ids) {
      Peer peer(get_peer_id(ctx, pid));
      Msg msg(ctx, Msg::POST, peer.email);
      msg.thread_id = thread.id;
      msg.post_body = post.body;
      insert(ctx.db.outbox, msg);
    }
  }
}
