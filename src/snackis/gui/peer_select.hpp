#ifndef SNACKIS_GUI_PEER_SELECT_HPP
#define SNACKIS_GUI_PEER_SELECT_HPP

#include "snackis/peer.hpp"
#include "snackis/gui/widget.hpp"

namespace snackis {
namespace gui {
  struct PeerSelect: Widget {
    Ctx &ctx;
    GtkWidget *box, *id_fld, *name_fld, *select_btn, *clear_btn;
    const db::Rec<Peer> *selected;
    
    PeerSelect(Ctx &ctx);
    GtkWidget *ptr() override;
  };
}}

#endif
