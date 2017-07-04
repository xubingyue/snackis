#ifndef SNACKIS_GUI_POST_SEARCH_HPP
#define SNACKIS_GUI_POST_SEARCH_HPP

#include <mutex>
#include <set>
#include <vector>

#include "snackis/post.hpp"
#include "snackis/gui/feed_select.hpp"
#include "snackis/gui/peer_select.hpp"
#include "snackis/gui/search_view.hpp"

namespace snackis {
namespace gui {
  struct PostSearch: SearchView<Post> {
    GtkWidget *id_fld, *body_fld, *min_time_fld, *max_time_fld;
    FeedSelect feed_fld;
    PeerSelect peer_fld;

    PostSearch(Ctx &ctx);
    void find() override;    
  };
}}

#endif
