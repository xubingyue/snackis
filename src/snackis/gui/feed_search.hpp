#ifndef SNACKIS_GUI_FEED_SEARCH_HPP
#define SNACKIS_GUI_FEED_SEARCH_HPP

#include <mutex>
#include <set>
#include <vector>

#include "snackis/gui/view.hpp"

namespace snackis {
namespace gui {
  struct FeedSearch: View {
    GtkListStore *peers, *feeds;
    GtkWidget *name, *peer, *active, *find, *list, *close;
    
    FeedSearch(Ctx &ctx);
  };
}}

#endif
