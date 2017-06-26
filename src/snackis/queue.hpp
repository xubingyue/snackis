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
    
    Queue(Ctx &ctx);
    Queue(Ctx &ctx, const db::Rec<Queue> &rec);   
    Queue(const Msg &msg);
  };

  opt<Queue> find_queue_id(Ctx &ctx, UId id);
  Queue get_queue_id(Ctx &ctx, UId id);
  void add_task(Queue &q, const Task &tsk);
}

#endif
