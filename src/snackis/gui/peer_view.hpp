#ifndef SNACKIS_GUI_PEER_VIEW_HPP
#define SNACKIS_GUI_PEER_VIEW_HPP

#include "snackis/peer.hpp"
#include "snackis/gui/rec_view.hpp"

namespace snackis {
namespace gui {
  struct PeerView: RecView<Peer> {
    GtkWidget *created_at_fld, *changed_at_fld, *name_fld, *email_fld;
    PeerView(const Peer &peer);
    bool save() override;
  };
}}

#endif
