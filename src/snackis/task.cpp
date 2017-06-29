#include <iostream>
#include "snackis/ctx.hpp"
#include "snackis/peer.hpp"
#include "snackis/project.hpp"
#include "snackis/task.hpp"

namespace snackis {
  Task::Task(Ctx &ctx):
    IdRec(ctx),
    owner_id(whoami(ctx).id),
    created_at(now()),
    done(false)
  { }

  Task::Task(Ctx &ctx, const db::Rec<Task> &rec): IdRec(ctx, false) {
    copy(*this, rec);
  }

  Task::Task(const Msg &msg):
    IdRec(msg.ctx, null_uid)
  {
  }

  opt<Task> find_task_id(Ctx &ctx, UId id) {
    db::Rec<Task> rec;
    set(rec, ctx.db.task_id, id);
    if (!load(ctx.db.tasks, rec)) { return nullopt; }
    return Task(ctx, rec);
  }

  Task get_task_id(Ctx &ctx, UId id) {
    auto found(find_task_id(ctx, id));
    
    if (!found) {
      ERROR(Db, fmt("Task id not found: %0", id));
    }

    return *found;
  }

  Feed get_feed(const Task &tsk) {
    Ctx &ctx(tsk.ctx);
    db::Rec<Feed> rec;
    db::set(rec, ctx.db.feed_id, tsk.id);
    Feed feed(ctx, rec);

    if (!db::load(ctx.db.feeds, feed)) {
      db::Trans trans(ctx);
      feed.name = fmt("Task %0", id_str(tsk));
      feed.visible = false;
      feed.peer_ids = tsk.peer_ids;
      db::insert(ctx.db.feeds, feed);
      db::commit(trans);
    }
    
    return feed;
  }

  void set_project(Task &tsk, Project &prj) {
    tsk.project_id = prj.id;
    tsk.peer_ids = prj.peer_ids;
  }
}
