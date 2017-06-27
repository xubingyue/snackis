#include <iostream>
#include "snackis/ctx.hpp"
#include "snackis/peer.hpp"
#include "snackis/queue.hpp"

namespace snackis {
  Queue::Queue(Ctx &ctx): Rec(ctx), id(true), owner_id(whoami(ctx).id)
  { }

  Queue::Queue(Ctx &ctx, const db::Rec<Queue> &rec): Rec(ctx) {
    copy(*this, rec);
  }

  Queue::Queue(const Msg &msg):
    Rec(msg.ctx),
    //id(msg.queue_id),
    owner_id(msg.from_id)
    //name(msg.queue_name),
    //info(msg.queue_info)
  { }

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

  bool add_task(Queue &q, Task &tsk) {
    Ctx &ctx(q.ctx);
    
    if (q.task_ids.insert(tsk.id).second) {
      db::upsert(ctx.db.queues, q);

      tsk.queue_ids.insert(q.id);
      db::upsert(ctx.db.tasks, tsk);

      QueueTask qt(q, tsk);
      db::insert(ctx.db.queue_tasks, qt);
      return true;
    }

    return false;
  }
}
