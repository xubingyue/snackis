#ifndef SNACKIS_GUI_FEED_VIEW_HPP
#define SNACKIS_GUI_FEED_VIEW_HPP

#include <mutex>
#include <set>
#include <vector>

#include "snackis/feed.hpp"
#include "snackis/gui/view.hpp"

namespace snackis {
namespace gui {
  struct FeedView: View {
    Feed feed;
    GtkListStore *peers, *feed_peers;
    GtkWidget *name, *peer_list, *peer_input, *peer, *add_peer, *remove_peer,
      *cancel, *save;
    
    FeedView(Ctx &ctx);
    void focus() override;
  };
}}

#endif
