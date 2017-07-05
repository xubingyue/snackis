#include "snackis/gui/queue_search.hpp"
#include "snackis/gui/queue_select.hpp"

namespace snackis {
namespace gui {
  QueueSelect::QueueSelect(Ctx &ctx): RecSelect<Queue>(ctx)
  { }

  SearchView<Queue> *QueueSelect::search() const {
      return new QueueSearch(ctx);
  }
}}
