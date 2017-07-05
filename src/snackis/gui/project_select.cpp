#include "snackis/gui/project_search.hpp"
#include "snackis/gui/project_select.hpp"

namespace snackis {
namespace gui {
  ProjectSelect::ProjectSelect(Ctx &ctx): RecSelect<Project>(ctx)
  { }

  SearchView<Project> *ProjectSelect::search() const {
      return new ProjectSearch(ctx);
  }
}}
