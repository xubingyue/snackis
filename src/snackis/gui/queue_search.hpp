#ifndef SNACKIS_GUI_QUEUE_SEARCH_HPP
#define SNACKIS_GUI_QUEUE_SEARCH_HPP

#include "snackis/queue.hpp"
#include "snackis/gui/peer_select.hpp"
#include "snackis/gui/search_view.hpp"

namespace snackis {
namespace gui {
  struct QueueSearch: SearchView<Queue> {
    GtkWidget *id_fld, *text_fld;
    PeerSelect peer_fld;

    QueueSearch(Ctx &ctx);
    void find() override;    
  };
}}

#endif
