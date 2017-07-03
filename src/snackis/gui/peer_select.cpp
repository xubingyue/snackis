#include "snackis/gui/peer_search.hpp"
#include "snackis/gui/peer_select.hpp"

namespace snackis {
namespace gui {
  PeerSelect::PeerSelect(Ctx &ctx): RecSelect<Peer>(ctx)
  { }

  SearchView<Peer> *PeerSelect::search() const {
      return new PeerSearch(ctx);
  }
}}
