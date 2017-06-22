#ifndef SNACKIS_GUI_POST_VIEW_HPP
#define SNACKIS_GUI_POST_VIEW_HPP

#include <vector>
#include <mutex>

#include "snackis/post.hpp"
#include "snackis/gui/view.hpp"

namespace snackis {
namespace gui {
  struct PostView: View {
    Post post;
    GtkListStore *feeds, *posts;
    GtkWidget *feed, *body_text, *body, *post_list, *save, *cancel;
    
    PostView(const Post &post);
  };
}}

#endif
