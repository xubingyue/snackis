#ifndef SNACKIS_GUI_PROJECT_SEARCH_HPP
#define SNACKIS_GUI_PROJECT_SEARCH_HPP

#include "snackis/project.hpp"
#include "snackis/gui/peer_select.hpp"
#include "snackis/gui/search_view.hpp"

namespace snackis {
namespace gui {
  struct ProjectSearch: SearchView<Project> {
    GtkWidget *id_fld, *tags_fld, *text_fld;
    PeerSelect peer_fld;

    ProjectSearch(Ctx &ctx);
    void find() override;    
  };
}}

#endif
