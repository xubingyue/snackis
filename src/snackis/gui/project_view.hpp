#ifndef SNACKIS_GUI_PROJECT_VIEW_HPP
#define SNACKIS_GUI_PROJECT_VIEW_HPP

#include <vector>
#include <mutex>

#include "snackis/project.hpp"
#include "snackis/gui/rec_view.hpp"

namespace snackis {
namespace gui {
  struct ProjectView: RecView<Project> {
    GtkWidget *name_fld, *active_fld, *info_fld;
    
    ProjectView(const Project &project);
    bool allow_save() const override;
    void save() override;
  };
}}

#endif
