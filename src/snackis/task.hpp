#ifndef SNACKIS_TASK_HPP
#define SNACKIS_TASK_HPP

#include <set>

#include "snackis/id_rec.hpp"
#include "snackis/core/str.hpp"
#include "snackis/core/time.hpp"
#include "snackis/core/uid.hpp"
#include "snackis/db/key.hpp"
#include "snackis/db/schema.hpp"
#include "snackis/db/rec_type.hpp"

namespace snackis {
  struct Task: IdRec {
    UId project_id, owner_id;
    Time created_at, changed_at;
    str name, info;
    std::set<str> tags;
    int64_t prio;
    bool done;
    Time done_at;
    std::set<UId> peer_ids;
    
    Task(Ctx &ctx);
    Task(Ctx &ctx, const db::Rec<Task> &rec);   
    Task(const Msg &msg);
  };

  extern db::Col<Task, UId>           task_id;
  extern db::Col<Task, UId>           task_project_id;
  extern db::Col<Task, UId>           task_owner_id;
  extern db::Col<Task, Time>          task_created_at, task_changed_at;
  extern db::Col<Task, str>           task_name, task_info;
  extern db::Col<Task, std::set<str>> task_tags;
  extern db::Col<Task, int64_t>       task_prio;
  extern db::Col<Task, bool>          task_done;
  extern db::Col<Task, Time>          task_done_at;
  extern db::Col<Task, std::set<UId>> task_peer_ids;

  extern db::Key<Task, UId> task_key;
  extern db::Schema<Task> task_cols;
  extern db::RecType<Task> task_type;

  void copy(Task &tsk, const Msg &msg);
  opt<Task> find_task_id(Ctx &ctx, UId id);
  Task get_task_id(Ctx &ctx, UId id);
  Feed get_feed(const Task &tsk);
  void set_project(Task &tsk, Project &project);
  void send(const Task &tsk, const Peer &pr);
  void send(const Task &tsk);
}

#endif
