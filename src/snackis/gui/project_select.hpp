#ifndef SNACKIS_GUI_PROJECT_SELECT_HPP
#define SNACKIS_GUI_PROJECT_SELECT_HPP

#include "snackis/project.hpp"
#include "snackis/gui/rec_select.hpp"

namespace snackis {
namespace gui {
  struct ProjectSelect: RecSelect<Project> {
    ProjectSelect(Ctx &ctx);
    SearchView<Project> *search() const override;
  };
}}

#endif
