#include "snackis/ctx.hpp"
#include "snackis/thread.hpp"

namespace snackis {
  Thread::Thread(Ctx &ctx): Rec(ctx), started_at(now()) { }

  Thread::Thread(const db::Table<Thread> &tbl, const db::Rec<Thread> &rec):
    Rec(dynamic_cast<Ctx &>(tbl.ctx)), id(false) {
    copy(tbl, *this, rec);
  }

  str history(const Thread &thread, size_t max) {
    Ctx &ctx(thread.ctx);
    db::Rec<Post> post_rec;
    set(post_rec, ctx.db.post_thread_id, thread.id);
    Stream out;
    //size_t len = 0;

    for (auto found(ctx.db.thread_posts.recs.lower_bound(post_rec));
	 found != ctx.db.thread_posts.recs.end();
	 found++) {
      post_rec.clear();
      copy(ctx.db.posts.key, post_rec, *found);
      load(ctx.db.posts, post_rec);
      Post post(ctx.db.posts, post_rec);
      //len += 1 + count(post.body.begin(), post.body.end(), '\n');
      //if (len > max) { break; }      
      load(ctx.db.peers, post.by);
      Peer peer(ctx.db.peers, post.by);
      out << fmt("%0 (%1)\n%2\n", peer.name, peer.email, post.body);
    }

    return out.str();
  }
}
