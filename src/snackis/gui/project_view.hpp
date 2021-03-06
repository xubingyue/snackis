#ifndef SNACKIS_GUI_PROJECT_VIEW_HPP
#define SNACKIS_GUI_PROJECT_VIEW_HPP

#include "snackis/project.hpp"
#include "snackis/gui/feed_history.hpp"
#include "snackis/gui/peer_list.hpp"
#include "snackis/gui/shared_view.hpp"

namespace snackis {
namespace gui {
  struct ProjectView: SharedView<Project> {
    GtkWidget *new_project_btn, *find_tasks_btn, *new_task_btn, *find_posts_btn,
      *post_btn, *name_fld, *tags_fld, *active_fld, *info_fld;
    PeerList peer_lst;
    FeedHistory post_lst;
    
    ProjectView(const Project &rec);
    bool save() override;
  };
}}

#endif
