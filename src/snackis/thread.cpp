#include "snackis/ctx.hpp"
#include "snackis/thread.hpp"

namespace snackis {
  Thread::Thread(Ctx &ctx): Rec(ctx), started_at(now()) { }

  Thread::Thread(const db::Table<Thread> &tbl, const db::Rec<Thread> &rec):
    Rec(dynamic_cast<Ctx &>(tbl.ctx)), id(false) {
    copy(tbl, *this, rec);
  }

  Thread get_thread_id(Ctx &ctx, UId id) {
    db::Rec<Thread> rec;
    set(rec, ctx.db.thread_id, id);

    if (!load(ctx.db.threads, rec)) {
      ERROR(Db, fmt("Thread id not found: %0", id));
    }

    return Thread(ctx.db.threads, rec);
  }

  std::vector<Post> last_posts(const Thread &thread, size_t max) {
    Ctx &ctx(thread.ctx);
    db::Rec<Post> post_rec;
    set(post_rec, ctx.db.post_thread_id, thread.id);
    set(post_rec, ctx.db.post_at, Time::max());
    std::vector<Post> posts;
    if (ctx.db.thread_posts.recs.empty()) { return posts; }
    auto found(ctx.db.thread_posts.recs.lower_bound(post_rec));
    if (found == ctx.db.thread_posts.recs.begin()) { return posts; }
    found--;
    
    while (posts.size() < max) {
      post_rec.clear();
      copy(ctx.db.posts.key, post_rec, *found);
      load(ctx.db.posts, post_rec);
      Post post(ctx.db.posts, post_rec);
      if (post.thread_id != thread.id) { break; }
      posts.push_back(post);
      if (found == ctx.db.thread_posts.recs.begin()) { break; }
      found--;
    }

    return posts;
  }
}
