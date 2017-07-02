#ifndef SNACKIS_GUI_FEED_SEARCH_HPP
#define SNACKIS_GUI_FEED_SEARCH_HPP

#include <mutex>
#include <set>
#include <vector>

#include "snackis/gui/view.hpp"
#include "snackis/gui/peer_select.hpp"

namespace snackis {
namespace gui {
  struct FeedSearch: View {
    GtkListStore *feeds;
    GtkWidget *name, *active, *find, *list, *close;
    PeerSelect peer_fld;
    FeedSearch(Ctx &ctx);
    void init() override;
  };
}}

#endif
