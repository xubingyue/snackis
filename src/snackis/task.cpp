#include <iostream>
#include "snackis/ctx.hpp"
#include "snackis/peer.hpp"
#include "snackis/project.hpp"
#include "snackis/task.hpp"

namespace snackis {
  Task::Task(Ctx &ctx, const Project &prj):
    Rec(ctx), project_id(prj.id), owner_id(whoami(ctx).id), created_at(now()),
    deadline(Time::max()), done(false)
  { }

  Task::Task(Ctx &ctx, const db::Rec<Task> &rec): Rec(ctx), id(false) {
    copy(*this, rec);
  }

  Task::Task(const Msg &msg):
    Rec(msg.ctx),
    //id(msg.task_id),
    owner_id(msg.from_id)
    //name(msg.task_name),
    //info(msg.task_info)
  { }

  opt<Task> find_task_id(Ctx &ctx, UId id) {
    db::Rec<Task> rec;
    set(rec, ctx.db.task_id, id);
    if (!load(ctx.db.tasks, rec)) { return nullopt; }
    return Task(ctx, rec);
  }

  Feed get_feed(const Task &tsk) {
    Ctx &ctx(tsk.ctx);
    db::Rec<Feed> feed_rec;
    db::set(feed_rec, ctx.db.feed_id, tsk.id);
    Feed feed(ctx, feed_rec);
    
    if (!db::load(ctx.db.feeds, feed)) {
      feed.name = fmt("Task %0", tsk.name);
      auto prj(get_project_id(ctx, tsk.project_id));
      feed.peer_ids = prj.peer_ids;
      db::upsert(ctx.db.feeds, feed);
    }

    return feed;
  }
}
