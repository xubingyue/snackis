#ifndef SNACKIS_TASK_HPP
#define SNACKIS_TASK_HPP

#include <set>

#include "snackis/id_rec.hpp"
#include "snackis/core/str.hpp"
#include "snackis/core/time.hpp"
#include "snackis/core/uid.hpp"

namespace snackis {
  struct Task: IdRec {
    UId project_id, owner_id;
    Time created_at;
    str name, info;
    bool done;
    std::set<UId> peer_ids;
    std::set<UId> queue_ids;
    
    Task(Ctx &ctx);
    Task(Ctx &ctx, const db::Rec<Task> &rec);   
    Task(const Msg &msg);
  };

  opt<Task> find_task_id(Ctx &ctx, UId id);
  Task get_task_id(Ctx &ctx, UId id);
  void set_project(Task &tsk, const Project &prj);
  Feed get_feed(const Task &tsk);
}

#endif
