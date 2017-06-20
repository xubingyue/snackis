#ifndef SNACKIS_GUI_FEED_VIEW_HPP
#define SNACKIS_GUI_FEED_VIEW_HPP

#include <vector>
#include <mutex>

#include "snackis/gui/view.hpp"

namespace snackis {
namespace gui {
  struct FeedView: View {
    GtkListStore *peers, *feed_peers;
    GtkWidget *name, *peer_list, *peer, *cancel, *save;
    
    FeedView(Ctx &ctx);
    void focus() override;
  };
}}

#endif
