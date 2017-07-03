#include "snackis/ctx.hpp"
#include "snackis/gui/peer_list.hpp"
#include "snackis/gui/peer_search.hpp"

namespace snackis {
namespace gui {
  PeerList::PeerList(Ctx &ctx, const str &lbl, std::set<UId> &ids):
    RecList<Peer>(ctx, lbl, ids)
  { }

  db::Table<Peer> &PeerList::table() const { return ctx.db.peers; }

  SearchView<Peer> *PeerList::search() const { return new PeerSearch(ctx); }
}}
