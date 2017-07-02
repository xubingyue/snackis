#ifndef SNACKIS_GUI_PEER_SEARCH_HPP
#define SNACKIS_GUI_PEER_SEARCH_HPP

#include <mutex>
#include <set>
#include <vector>

#include "snackis/peer.hpp"
#include "snackis/gui/search_view.hpp"

namespace snackis {
namespace gui {
  struct PeerSearch: SearchView<Peer> {
    GtkWidget *id_fld, *name_fld, *email_fld;
    
    PeerSearch(Ctx &ctx);
    void init() override;
    void find() override;
    void activate(const db::Rec<Peer> &rec) override;
  };
}}

#endif
