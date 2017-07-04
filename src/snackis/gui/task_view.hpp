#ifndef SNACKIS_GUI_TASK_VIEW_HPP
#define SNACKIS_GUI_TASK_VIEW_HPP

#include <vector>
#include <mutex>

#include "snackis/queue.hpp"
#include "snackis/task.hpp"
#include "snackis/gui/rec_view.hpp"

namespace snackis {
namespace gui {
  struct TaskView: RecView<Task> {
    GtkListStore *project_store;
    GtkWidget *find_posts_btn, *post_btn, *project_fld, *edit_project_btn,
      *name_fld, *done_fld, *info_fld;
    opt<Queue> queue;
    
    TaskView(const Task &rec);
    bool allow_save() const override;
    bool save() override;
  };
}}

#endif
