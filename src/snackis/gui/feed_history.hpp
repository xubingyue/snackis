#ifndef SNACKIS_GUI_FEED_HISTORY_HPP
#define SNACKIS_GUI_FEED_HISTORY_HPP

#include "snackis/core/time.hpp"
#include "snackis/core/uid.hpp"
#include "snackis/gui/widget.hpp"

namespace snackis {
  struct Ctx;
  struct Feed;
  
namespace gui {
  struct FeedHistory: Widget {
    Ctx &ctx;
    GtkListStore *store;
    GtkWidget *lst;
    
    FeedHistory(Ctx &ctx, const str &lbl);
    GtkWidget *ptr() override;
  };

  void clear(FeedHistory &w);
  void load(FeedHistory &w, const Feed &fd, Time start);
}}

#endif
