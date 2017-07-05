#ifndef SNACKIS_GUI_ENCRYPT_HPP
#define SNACKIS_GUI_ENCRYPT_HPP

#include "snackis/gui/peer_select.hpp"
#include "snackis/gui/view.hpp"

namespace snackis {
namespace gui {
  struct Encrypt: View {
    GtkWidget *source, *target, *encode, *save_btn, *cancel_btn;
    PeerSelect peer_fld;
    Encrypt(Ctx &ctx);
  };
}}

#endif
