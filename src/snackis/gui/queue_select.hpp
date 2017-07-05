#ifndef SNACKIS_GUI_QUEUE_SELECT_HPP
#define SNACKIS_GUI_QUEUE_SELECT_HPP

#include "snackis/queue.hpp"
#include "snackis/gui/rec_select.hpp"

namespace snackis {
namespace gui {
  struct QueueSelect: RecSelect<Queue> {
    QueueSelect(Ctx &ctx);
    SearchView<Queue> *search() const override;
  };
}}

#endif
