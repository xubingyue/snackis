#ifndef SNACKIS_GUI_FEED_VIEW_HPP
#define SNACKIS_GUI_FEED_VIEW_HPP

#include <mutex>
#include <set>
#include <vector>

#include "snackis/feed.hpp"
#include "snackis/gui/rec_view.hpp"

namespace snackis {
namespace gui {
  struct FeedView: RecView<Feed> {
    GtkListStore *peer_store, *feed_peer_store, *post_store;
    GtkWidget *name_fld, *active_fld, *info_fld, *peer_lst, *peer_fld,
      *add_peer_btn, *post_lst, *post_fld;
    
    FeedView(const Feed &feed);
    bool allow_save() const override;
    void save() override;
  };
}}

#endif
