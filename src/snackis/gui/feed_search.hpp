#ifndef SNACKIS_GUI_FEED_SEARCH_HPP
#define SNACKIS_GUI_FEED_SEARCH_HPP

#include <mutex>
#include <set>
#include <vector>

#include "snackis/feed.hpp"
#include "snackis/gui/search_view.hpp"
#include "snackis/gui/peer_select.hpp"

namespace snackis {
namespace gui {
  struct FeedSearch: SearchView<Feed> {
    GtkWidget *id_fld, *name_fld, *active_fld;
    PeerSelect peer_fld;
    FeedSearch(Ctx &ctx);
    void init() override;
    void find() override;    
  };
}}

#endif
