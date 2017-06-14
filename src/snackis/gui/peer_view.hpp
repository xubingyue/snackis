#ifndef SNACKIS_GUI_PEER_VIEW_HPP
#define SNACKIS_GUI_PEER_VIEW_HPP

#include <vector>
#include <mutex>

#include "snackis/peer.hpp"
#include "snackis/gui/view.hpp"

namespace snackis {
namespace gui {
  struct PeerView: public View {
    Peer peer;
    GtkWidget *name, *email, *cancel, *save;
    
    PeerView(const Peer &peer);
    void focus() override;
  };
}}

#endif
