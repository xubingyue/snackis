#ifndef SNACKIS_GUI_PEER_LIST_HPP
#define SNACKIS_GUI_PEER_LIST_HPP

#include "snackis/peer.hpp"
#include "snackis/gui/rec_list.hpp"

namespace snackis {
namespace gui {
  struct PeerList: RecList<Peer> {
    PeerList(Ctx &ctx, const str &lbl, std::set<UId> &ids);
    db::Table<Peer, UId> &table() const override;
    SearchView<Peer> *search() const override;
  };
}}

#endif
