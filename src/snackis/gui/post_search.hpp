#ifndef SNACKIS_GUI_POST_SEARCH_HPP
#define SNACKIS_GUI_POST_SEARCH_HPP

#include <mutex>
#include <set>
#include <vector>

#include "snackis/feed.hpp"
#include "snackis/gui/peer_select.hpp"
#include "snackis/gui/view.hpp"


namespace snackis {
namespace gui {
  struct PostSearch: View {
    GtkListStore *feed_store, *peers, *posts;
    GtkWidget *body, *min_time, *max_time, *feed_fld, *find, *list, *close;
    PeerSelect peer_fld;
    opt<Feed> feed;

    PostSearch(Ctx &ctx);
    void init() override;
  };
}}

#endif
