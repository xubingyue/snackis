#ifndef SNACKIS_GUI_PEER_SELECT_HPP
#define SNACKIS_GUI_PEER_SELECT_HPP

#include "snackis/peer.hpp"
#include "snackis/gui/rec_select.hpp"

namespace snackis {
namespace gui {
  struct PeerSelect: RecSelect<Peer> {
    PeerSelect(Ctx &ctx);
    SearchView<Peer> *search() const override;
  };
}}

#endif
