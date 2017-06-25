#include <iostream>
#include "snackis/ctx.hpp"
#include "snackis/peer.hpp"
#include "snackis/project.hpp"
#include "snackis/task.hpp"

namespace snackis {
  Task::Task(Project &prj):
    Rec(prj.ctx),
    id(true),
    project_id(prj.id),
    owner_id(whoami(ctx).id),
    peer_ids(prj.peer_ids),
    deadline(Time::max()),
    done(false)
  { }

  Task::Task(Ctx &ctx, const db::Rec<Task> &rec): Rec(ctx) {
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

  Task get_task_id(Ctx &ctx, UId id) {
    auto found(find_task_id(ctx, id));
    
    if (!found) {
      ERROR(Db, fmt("Task id not found: %0", id));
    }

    return *found;
  }
}
