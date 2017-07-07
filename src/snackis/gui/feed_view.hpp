#ifndef SNACKIS_GUI_FEED_VIEW_HPP
#define SNACKIS_GUI_FEED_VIEW_HPP

#include "snackis/feed.hpp"
#include "snackis/gui/peer_list.hpp"
#include "snackis/gui/rec_view.hpp"

namespace snackis {
namespace gui {
  struct FeedView: RecView<Feed> {
    GtkWidget *find_posts_btn, *post_btn, *name_fld, *tags_fld, *active_fld,
      *info_fld;
    PeerList peer_lst;
    
    FeedView(const Feed &feed);
    bool save() override;
  };
}}

#endif
