#ifndef SNACKIS_GUI_POST_VIEW_HPP
#define SNACKIS_GUI_POST_VIEW_HPP

#include <vector>
#include <mutex>

#include "snackis/post.hpp"
#include "snackis/gui/rec_view.hpp"

namespace snackis {
namespace gui {
  struct PostView: RecView<Post> {
    GtkListStore *feed_store, *post_store;
    GtkWidget *feed_fld, *edit_feed_btn, *body_fld, *post_lst;
    
    PostView(const Post &post);
    bool allow_save() const override;
    bool save() override;
  };
}}

#endif
