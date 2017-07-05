#ifndef SNACKIS_QUEUE_HPP
#define SNACKIS_QUEUE_HPP

#include <set>

#include "snackis/id_rec.hpp"
#include "snackis/core/str.hpp"
#include "snackis/core/uid.hpp"

namespace snackis {
  struct Feed;
  struct Msg;
  struct Task;
  
  struct Queue: IdRec {
    UId owner_id;
    Time created_at, changed_at;
    str name, info;
    std::set<UId> peer_ids;
    
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
  Feed get_feed(const Queue &q);
}

#endif
