#ifndef SNACKIS_QUEUE_HPP
#define SNACKIS_QUEUE_HPP

#include <set>

#include "snackis/rec.hpp"
#include "snackis/core/str.hpp"
#include "snackis/core/uid.hpp"

namespace snackis {
  struct Task;
  
  struct Queue: Rec {
    UId id, owner_id;
    str name, info;
    std::set<UId> peer_ids;
    std::set<UId> task_ids;
    
    Queue(Ctx &ctx);
    Queue(Ctx &ctx, const db::Rec<Queue> &rec);   
    Queue(const Msg &msg);
  };

  struct QueueTask: Rec {
    UId id, queue_id;
    Time at;
    
    QueueTask(const Queue &q, const Task &tsk);
    QueueTask(Ctx &ctx, const db::Rec<QueueTask> &rec);   
  };
  
  opt<Queue> find_queue_id(Ctx &ctx, UId id);
  Queue get_queue_id(Ctx &ctx, UId id);
  bool add_task(Queue &q, Task &tsk);
}

#endif
