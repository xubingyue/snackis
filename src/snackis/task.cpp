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
    changed_at(created_at),
    prio(0),
    done(false)
  { }

  Task::Task(Ctx &ctx, const db::Rec<Task> &rec):
    IdRec(ctx, false),
    prio(0)
  {
    db::copy(*this, rec);
  }

  Task::Task(const Msg &msg):
    IdRec(msg.ctx, *db::get(msg.task, msg.ctx.db.task_id)),
    owner_id(msg.from_id),
    created_at(now()),
    changed_at(created_at),
    prio(0)
  {
    copy(*this, msg);
    peer_ids.insert(msg.from_id);
  }

  void copy(Task &tsk, const Msg &msg) {
    Ctx &ctx(tsk.ctx);
    ctx.db.task_id.copy(tsk, msg.task);
    ctx.db.task_project_id.copy(tsk, msg.task);
    ctx.db.task_name.copy(tsk, msg.task);
    ctx.db.task_info.copy(tsk, msg.task);
    ctx.db.task_done.copy(tsk, msg.task);
    ctx.db.task_done_at.copy(tsk, msg.task);
  }

  opt<Task> find_task_id(Ctx &ctx, UId id) {
    db::Rec<Task> rec;
    set(rec, ctx.db.task_id, id);
    if (!load(ctx.db.tasks, rec)) { return nullopt; }
    return Task(ctx, rec);
  }

  Task get_task_id(Ctx &ctx, UId id) {
    auto found(find_task_id(ctx, id));
    CHECK(found, _);
    return *found;
  }

  Feed get_feed(const Task &tsk) {
    Ctx &ctx(tsk.ctx);
    db::Rec<Feed> rec;
    db::set(rec, ctx.db.feed_id, tsk.id);
    Feed fd(ctx, rec);

    if (!db::load(ctx.db.feeds, fd)) {
      db::Trans trans(ctx);
      TRY(try_create);
      fd.name = fmt("Task %0", id_str(tsk));
      fd.tags = tsk.tags;
      fd.owner_id = tsk.owner_id;
      fd.active = true;
      fd.visible = false;
      fd.peer_ids = tsk.peer_ids;
      db::insert(ctx.db.feeds, fd);

      if (try_create.errors.empty()) {
	db::commit(trans, fmt("Created Task feed %0", id_str(fd)));
      }
    }
    
    return fd;
  }

  void set_project(Task &tsk, Project &prj) {
    tsk.project_id = prj.id;
    tsk.peer_ids = prj.peer_ids;
  }

  static void send(const Task &tsk, const Peer &pr) {
    Ctx &ctx(tsk.ctx);
    Msg msg(ctx, Msg::TASK);
    msg.to = pr.email;
    msg.to_id = pr.id;

    auto prj(get_project_id(ctx, tsk.project_id));
    ctx.db.project_id.copy(msg.project, prj);
    ctx.db.project_name.copy(msg.project, prj);
    ctx.db.project_info.copy(msg.project, prj);
    ctx.db.project_active.copy(msg.project, prj);
    
    ctx.db.task_id.copy(msg.task, tsk);
    ctx.db.task_project_id.copy(msg.task, tsk);
    ctx.db.task_name.copy(msg.task, tsk);
    ctx.db.task_info.copy(msg.task, tsk);
    ctx.db.task_done.copy(msg.task, tsk);
    ctx.db.task_done_at.copy(msg.task, tsk);

    insert(ctx.db.outbox, msg);
  }

  void send(const Task &tsk) {
    Ctx &ctx(tsk.ctx);
    for (auto &pid: tsk.peer_ids) {
      auto pr(find_peer_id(ctx, pid));
      if (pr) { send(tsk, *pr); }
    }
  }
}
