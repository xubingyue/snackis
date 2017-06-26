#ifndef SNACKIS_GUI_TASK_VIEW_HPP
#define SNACKIS_GUI_TASK_VIEW_HPP

#include <vector>
#include <mutex>

#include "snackis/task.hpp"
#include "snackis/gui/rec_view.hpp"

namespace snackis {
namespace gui {
  struct TaskView: RecView<Task> {
    GtkListStore *project_store;
    GtkWidget *project_fld, *edit_project_btn, *name_fld, *info_fld;
    
    TaskView(const Task &task);
    void on_save() override;
  };
}}

#endif
