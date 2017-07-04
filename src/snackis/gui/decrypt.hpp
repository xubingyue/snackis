#ifndef SNACKIS_GUI_DECRYPT_HPP
#define SNACKIS_GUI_DECRYPT_HPP

#include <vector>
#include <mutex>

#include "snackis/gui/peer_select.hpp"
#include "snackis/gui/view.hpp"

namespace snackis {
namespace gui {
  struct Decrypt: View {
    GtkWidget *source, *target, *decode, *save, *cancel;
    PeerSelect peer_fld;
    Decrypt(Ctx &ctx);
  };
}}

#endif
