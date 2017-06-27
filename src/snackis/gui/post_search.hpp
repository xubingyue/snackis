#ifndef SNACKIS_GUI_POST_SEARCH_HPP
#define SNACKIS_GUI_POST_SEARCH_HPP

#include <mutex>
#include <set>
#include <vector>

#include "snackis/feed.hpp"
#include "snackis/gui/view.hpp"

namespace snackis {
namespace gui {
  struct PostSearch: View {
    GtkListStore *feed_store, *peers, *posts;
    GtkWidget *feed_fld, *body, *min_time, *max_time, *peer, *find, *list,
      *close;
    opt<Feed> feed;
    PostSearch(Ctx &ctx);
  };
}}

#endif
