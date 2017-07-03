#ifndef SNACKIS_GUI_TASK_SEARCH_HPP
#define SNACKIS_GUI_TASK_SEARCH_HPP

#include <mutex>
#include <set>
#include <vector>

#include "snackis/project.hpp"
#include "snackis/task.hpp"
#include "snackis/gui/peer_select.hpp"
#include "snackis/gui/search_view.hpp"

namespace snackis {
namespace gui {
  struct TaskSearch: SearchView<Task> {
    GtkListStore *project_store, *queue_store;
    GtkWidget *id_fld, *text_fld, *done_fld, *project_fld, *queue_fld;
    PeerSelect peer_fld;
    opt<Project> project;
    opt<Queue> queue;
    
    TaskSearch(Ctx &ctx);
    void init() override;
    void find() override;    
  };
}}

#endif
