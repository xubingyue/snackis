#ifndef SNACKIS_GUI_POST_SEARCH_HPP
#define SNACKIS_GUI_POST_SEARCH_HPP

#include <mutex>
#include <set>
#include <vector>

#include "snackis/post.hpp"
#include "snackis/gui/peer_select.hpp"
#include "snackis/gui/search_view.hpp"

namespace snackis {
namespace gui {
  struct PostSearch: SearchView<Post> {
    GtkListStore *feed_store, *peers;
    GtkWidget *id_fld, *body_fld, *min_time_fld, *max_time_fld, *feed_fld;
    PeerSelect peer_fld;
    opt<Feed> feed;

    PostSearch(Ctx &ctx);
    void init() override;
    void find() override;    
  };
}}

#endif
