#ifndef SNACKIS_GUI_QUEUE_LIST_HPP
#define SNACKIS_GUI_QUEUE_LIST_HPP

#include "snackis/queue.hpp"
#include "snackis/gui/rec_list.hpp"

namespace snackis {
namespace gui {
  struct QueueList: RecList<Queue> {
    QueueList(Ctx &ctx, const str &lbl, std::set<UId> &ids);
    db::Table<Queue> &table() const override;
    SearchView<Queue> *search() const override;
  };
}}

#endif
