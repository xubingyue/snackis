#include "snackis/ctx.hpp"
#include "snackis/peer.hpp"
#include "snackis/post.hpp"

namespace snackis {
  static Thread get_thread(const Msg &msg) {
    Ctx &ctx(msg.ctx);
    db::Rec<Thread> thread_rec;
    set(thread_rec, ctx.db.thread_id, msg.thread_id);
    bool exists = load(ctx.db.threads, thread_rec) ? true : false;
    Thread thread(ctx.db.threads, thread_rec);
    
    if (!exists) {
      thread.subj = msg.thread_subj;
      Peer peer(get_peer_email(ctx, msg.from));
      copy(ctx.db.peers.key, thread.started_by, peer);
      thread.started_at = now();
      insert(ctx.db.threads, thread);
    }

    return thread;
  }

  Post::Post(Thread &thread): Rec(thread.ctx), thread_id(thread.id)
  { }

  Post::Post(const Msg &msg):
    Rec(msg.ctx),
    thread_id(get_thread(msg).id), at(msg.post_at), body(msg.post_body) {
    Peer peer(get_peer_email(ctx, msg.from));
    copy(ctx.db.peers.key, by, peer);
  }

  Post::Post(const db::Table<Post> &tbl, const db::Rec<Post> &rec):
    Rec(dynamic_cast<Ctx &>(tbl.ctx)), id(false) {
    copy(tbl, *this, rec);
  }
  
  void post_msgs(const Post &post) {
    Ctx &ctx(post.ctx);
    Thread thread(get_thread_id(ctx, post.thread_id));
    
    for (auto &pid: thread.peer_ids) {
      Peer peer(get_peer_id(ctx, pid));
      Msg msg(ctx, Msg::POST, peer.email);
      msg.thread_id = thread.id;
      msg.thread_subj = thread.subj;
      msg.post_at = post.at;
      msg.post_body = post.body;
      insert(ctx.db.outbox, msg);
    }
  }
}
