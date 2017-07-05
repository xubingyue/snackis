#include "snackis/ctx.hpp"
#include "snackis/gui/queue_list.hpp"
#include "snackis/gui/queue_search.hpp"

namespace snackis {
namespace gui {
  QueueList::QueueList(Ctx &ctx, const str &lbl, std::set<UId> &ids):
    RecList<Queue>(ctx, lbl, ids)
  { }

  db::Table<Queue> &QueueList::table() const { return ctx.db.queues; }

  SearchView<Queue> *QueueList::search() const { return new QueueSearch(ctx); }
}}
