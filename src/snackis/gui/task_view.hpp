#ifndef SNACKIS_GUI_TASK_VIEW_HPP
#define SNACKIS_GUI_TASK_VIEW_HPP

#include "snackis/task.hpp"
#include "snackis/gui/project_select.hpp"
#include "snackis/gui/rec_view.hpp"

namespace snackis {
namespace gui {
  struct TaskView: RecView<Task> {
    GtkListStore *project_store;
    GtkWidget *find_posts_btn, *post_btn, *edit_project_btn,
      *name_fld, *done_fld, *tags_fld, *info_fld;
    ProjectSelect project_fld;
    
    TaskView(const Task &rec);
    bool allow_save() const override;
    bool save() override;
  };
}}

#endif
