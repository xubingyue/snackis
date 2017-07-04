#ifndef SNACKIS_GUI_PROJECT_VIEW_HPP
#define SNACKIS_GUI_PROJECT_VIEW_HPP

#include <vector>
#include <mutex>

#include "snackis/project.hpp"
#include "snackis/gui/peer_list.hpp"
#include "snackis/gui/rec_view.hpp"

namespace snackis {
namespace gui {
  struct ProjectView: RecView<Project> {
    GtkWidget *find_tasks_btn, *new_task_btn, *find_posts_btn, *post_btn, *name_fld,
      *active_fld, *info_fld;
    PeerList peer_lst;
    
    ProjectView(const Project &rec);
    bool allow_save() const override;
    bool save() override;
  };
}}

#endif
