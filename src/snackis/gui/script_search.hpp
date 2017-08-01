#ifndef SNACKIS_GUI_SCRIPT_SEARCH_HPP
#define SNACKIS_GUI_SCRIPT_SEARCH_HPP

#include "snackis/script.hpp"
#include "snackis/gui/peer_select.hpp"
#include "snackis/gui/search_view.hpp"

namespace snackis {
namespace gui {
  struct ScriptSearch: SearchView<Script> {
    GtkWidget *id_fld, *tags_fld, *code_fld;
    PeerSelect peer_fld;

    ScriptSearch(Ctx &ctx);
    void find() override;    
  };
}}

#endif
