#ifndef SNACKIS_GUI_TASK_SEARCH_HPP
#define SNACKIS_GUI_TASK_SEARCH_HPP

#include <mutex>
#include <set>
#include <vector>

#include "snackis/gui/view.hpp"

namespace snackis {
namespace gui {
  struct TaskSearch: View {
    GtkListStore *peers, *tasks;
    GtkWidget *text_fld, *done_fld, *peer_fld, *find_btn, *lst, *close_btn;
    
    TaskSearch(Ctx &ctx);
  };
}}

#endif
