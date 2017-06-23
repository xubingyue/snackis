#ifndef SNACKIS_GUI_POST_SEARCH_HPP
#define SNACKIS_GUI_POST_SEARCH_HPP

#include <mutex>
#include <set>
#include <vector>

#include "snackis/gui/view.hpp"

namespace snackis {
namespace gui {
  struct PostSearch: View {
    GtkListStore *peers, *posts;
    GtkWidget *feed_name, *active, *body, *min_time, *max_time, *peer, *find, *list,
      *close;
    
    PostSearch(Ctx &ctx);
  };
}}

#endif
