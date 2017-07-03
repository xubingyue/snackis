#ifndef SNACKIS_GUI_PEER_SELECT_HPP
#define SNACKIS_GUI_PEER_SELECT_HPP

#include "snackis/peer.hpp"
#include "snackis/gui/widget.hpp"

namespace snackis {
namespace gui {
  struct PeerSelect: Widget {
    Ctx &ctx;
    GtkWidget *box, *id_fld, *name_fld, *search_btn, *clear_btn;
    opt<Peer> selected;
    
    PeerSelect(Ctx &ctx);
    GtkWidget *ptr() override;
  };

  void select(PeerSelect &v, const opt<Peer> &peer);
}}

#endif
