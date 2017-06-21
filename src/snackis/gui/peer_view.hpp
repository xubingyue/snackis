#ifndef SNACKIS_GUI_PEER_VIEW_HPP
#define SNACKIS_GUI_PEER_VIEW_HPP

#include <mutex>
#include <vector>

#include "snackis/peer.hpp"
#include "snackis/gui/view.hpp"

namespace snackis {
namespace gui {
  struct PeerView: View {
    Peer peer;
    GtkWidget *name, *email, *save, *cancel;
    
    PeerView(const Peer &peer);
    void focus() override;
  };
}}

#endif
