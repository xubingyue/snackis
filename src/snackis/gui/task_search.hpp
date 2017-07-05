#ifndef SNACKIS_GUI_TASK_SEARCH_HPP
#define SNACKIS_GUI_TASK_SEARCH_HPP

#include "snackis/task.hpp"
#include "snackis/gui/peer_select.hpp"
#include "snackis/gui/project_select.hpp"
#include "snackis/gui/queue_select.hpp"
#include "snackis/gui/search_view.hpp"

namespace snackis {
namespace gui {
  struct TaskSearch: SearchView<Task> {
    GtkWidget *id_fld, *text_fld, *done_fld;
    ProjectSelect project_fld;
    QueueSelect queue_fld;
    PeerSelect peer_fld;
    
    TaskSearch(Ctx &ctx);
    void find() override;    
  };
}}

#endif
