#ifndef SNACKIS_UI_POST_FORM_HPP
#define SNACKIS_UI_POST_FORM_HPP

#include <set>

#include "snackis/peer.hpp"
#include "snackis/ui/view_form.hpp"
#include "snackis/ui/enum_field.hpp"
#include "snackis/ui/peer_panel.hpp"
#include "snackis/ui/text_field.hpp"

namespace snackis {
namespace ui {
  struct PostForm: public ViewForm {
    using CmpPeer = func<bool (const db::Rec<Peer> &, const db::Rec<Peer> &)>;

    std::set<db::Rec<Peer>, CmpPeer> peers;

    EnumField<UId> thread;
    TextField subject;
    PeerPanel peer;
    TextField send_to;
    TextField body;
    PostForm(View &view, Footer &ftr);
  };

  bool run(PostForm &frm);
}}

#endif
