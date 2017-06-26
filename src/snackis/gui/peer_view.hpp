#ifndef SNACKIS_GUI_PEER_VIEW_HPP
#define SNACKIS_GUI_PEER_VIEW_HPP

#include <mutex>
#include <vector>

#include "snackis/peer.hpp"
#include "snackis/gui/rec_view.hpp"

namespace snackis {
namespace gui {
  struct PeerView: RecView<Peer> {
    GtkWidget *name_fld, *email_fld;
    
    PeerView(const Peer &peer);
    void save() override;
  };
}}

#endif
