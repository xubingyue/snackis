#include <iostream>
#include "snackis/ctx.hpp"
#include "snackis/peer.hpp"
#include "snackis/queue.hpp"

namespace snackis {
  Queue::Queue(Ctx &ctx):
    IdRec(ctx),
    owner_id(whoami(ctx).id),
    created_at(now()),
    changed_at(created_at)
  { }

  Queue::Queue(Ctx &ctx, const db::Rec<Queue> &rec): IdRec(ctx, null_uid) {
    copy(*this, rec);
  }

  QueueTask::QueueTask(const Queue &q, const Task &tsk):
    Rec(q.ctx), id(tsk.id), queue_id(q.id), at(now())
  { }

  QueueTask::QueueTask(Ctx &ctx, const db::Rec<QueueTask> &rec): Rec(ctx) {
    copy(*this, rec);
  }

  opt<Queue> find_queue_id(Ctx &ctx, UId id) {
    db::Rec<Queue> rec;
    set(rec, ctx.db.queue_id, id);
    if (!load(ctx.db.queues, rec)) { return nullopt; }
    return Queue(ctx, rec);
  }

  Queue get_queue_id(Ctx &ctx, UId id) {
    auto found(find_queue_id(ctx, id));
    
    if (!found) {
      ERROR(Db, fmt("Queue id not found: %0", id));
    }

    return *found;
  }

  Feed get_feed(const Queue &q) {
    Ctx &ctx(q.ctx);
    db::Rec<Feed> rec;
    db::set(rec, ctx.db.feed_id, q.id);
    Feed feed(ctx, rec);

    if (!db::load(ctx.db.feeds, feed)) {
      db::Trans trans(ctx);
      TRY(try_create);
      feed.name = fmt("Queue %0", id_str(q));
      feed.visible = false;
      feed.peer_ids = q.peer_ids;
      db::insert(ctx.db.feeds, feed);
      if (try_create.errors.empty()) { db::commit(trans); }
    }
    
    return feed;
  }

  void send(const Queue &q)
  { }
}
