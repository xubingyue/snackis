#ifndef SNACKIS_GUI_POST_VIEW_HPP
#define SNACKIS_GUI_POST_VIEW_HPP

#include "snackis/post.hpp"
#include "snackis/gui/feed_history.hpp"
#include "snackis/gui/feed_select.hpp"
#include "snackis/gui/shared_view.hpp"

namespace snackis {
namespace gui {
  struct PostView: SharedView<Post> {
    GtkWidget *post_btn, *find_replies_btn, *reply_btn, *edit_feed_btn, *tags_fld,
      *body_fld;
    FeedSelect feed_fld;
    FeedHistory post_lst;
    PostView(const Post &post);
    bool allow_save() const override;
    bool save() override;
  };
}}

#endif
