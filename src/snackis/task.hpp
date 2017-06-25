#ifndef SNACKIS_TASK_HPP
#define SNACKIS_TASK_HPP

#include <set>

#include "snackis/rec.hpp"
#include "snackis/core/str.hpp"
#include "snackis/core/time.hpp"
#include "snackis/core/uid.hpp"

namespace snackis {
  struct Task: Rec {
    UId id;
    UId project_id;
    UId owner_id;
    Time created_at;
    str name;
    str info;
    std::set<UId> peer_ids;
    Time deadline;
    bool done;
    
    Task(Project &prj);
    Task(Ctx &ctx, const db::Rec<Task> &rec);   
    Task(const Msg &msg);
  };

  opt<Task> find_task_id(Ctx &ctx, UId id);
  Task get_task_id(Ctx &ctx, UId id);
  Feed get_feed(const Task &prj);
}

#endif
