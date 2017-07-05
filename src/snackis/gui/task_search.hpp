#ifndef SNACKIS_GUI_TASK_SEARCH_HPP
#define SNACKIS_GUI_TASK_SEARCH_HPP

#include "snackis/project.hpp"
#include "snackis/task.hpp"
#include "snackis/gui/peer_select.hpp"
#include "snackis/gui/queue_select.hpp"
#include "snackis/gui/search_view.hpp"

namespace snackis {
namespace gui {
  struct TaskSearch: SearchView<Task> {
    GtkListStore *project_store;
    GtkWidget *id_fld, *text_fld, *done_fld, *project_fld;
    QueueSelect queue_fld;
    PeerSelect peer_fld;
    opt<Project> project;
    
    TaskSearch(Ctx &ctx);
    void find() override;    
  };
}}

#endif
