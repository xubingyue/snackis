#ifndef SNACKIS_GUI_PROJECT_VIEW_HPP
#define SNACKIS_GUI_PROJECT_VIEW_HPP

#include <vector>
#include <mutex>

#include "snackis/project.hpp"
#include "snackis/gui/view.hpp"

namespace snackis {
namespace gui {
  struct ProjectView: View {
    Project project;
    GtkWidget *name, *info_text, *info, *save, *cancel;
    
    ProjectView(const Project &project);
  };
}}

#endif
