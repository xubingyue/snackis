#ifndef SNACKIS_GUI_POST_VIEW_HPP
#define SNACKIS_GUI_POST_VIEW_HPP

#include <vector>
#include <mutex>

#include "snackis/post.hpp"
#include "snackis/gui/feed_select.hpp"
#include "snackis/gui/rec_view.hpp"

namespace snackis {
namespace gui {
  struct PostView: RecView<Post> {
    GtkListStore *post_store;
    GtkWidget *edit_feed_btn, *body_fld, *post_lst;
    FeedSelect feed_fld;
    
    PostView(const Post &post);
    void init() override;
    bool allow_save() const override;
    bool save() override;
  };
}}

#endif
