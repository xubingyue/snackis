#ifndef SNACKIS_GUI_TASK_VIEW_HPP
#define SNACKIS_GUI_TASK_VIEW_HPP

#include "snackis/task.hpp"
#include "snackis/gui/feed_history.hpp"
#include "snackis/gui/project_select.hpp"
#include "snackis/gui/shared_view.hpp"

namespace snackis {
namespace gui {
  struct TaskView: SharedView<Task> {
    GtkListStore *project_store;
    GtkWidget *find_posts_btn, *post_btn, *project_btn,
      *name_fld, *prio_fld, *done_fld, *tags_fld, *info_fld;
    ProjectSelect project_fld;
    FeedHistory post_lst;
    
    TaskView(const Task &rec);
    bool allow_save() const override;
    void load() override;
    bool save() override;
  };
}}

#endif
