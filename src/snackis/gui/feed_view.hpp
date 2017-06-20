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
    GtkListStore *peers, *feed_peers, *posts;
    GtkWidget *name, *peer_list, *remove_peers, *peer_input, *peer, *add_peer,
      *post_list, *new_post_text, *new_post, *cancel, *save;
    
    FeedView(Ctx &ctx);
    void focus() override;
  };
}}

#endif
