#ifndef SNACKIS_GUI_TASK_SEARCH_HPP
#define SNACKIS_GUI_TASK_SEARCH_HPP

#include <mutex>
#include <set>
#include <vector>

#include "snackis/project.hpp"
#include "snackis/gui/view.hpp"

namespace snackis {
namespace gui {
  struct TaskSearch: View {
    GtkListStore *project_store, *queue_store, *peers, *tasks;
    GtkWidget *text_fld, *done_fld, *project_fld, *queue_fld, *peer_fld, *find_btn,
      *lst, *close_btn;
    opt<Project> project;
    opt<Queue> queue;
    opt<Peer> peer;
    
    TaskSearch(Ctx &ctx);
    void init() override;
  };
}}

#endif
