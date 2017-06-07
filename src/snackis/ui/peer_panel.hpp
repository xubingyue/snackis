#ifndef SNACKIS_UI_PEER_PANEL_HPP
#define SNACKIS_UI_PEER_PANEL_HPP

#include "snackis/core/uid.hpp"
#include "snackis/ui/enum_field.hpp"

namespace snackis {
namespace ui {
  struct PeerPanel {
    opt<UId> selected;
    
    EnumField<UId> name, email;
    PeerPanel(Form &frm, const str &prefix);
  };
}}

#endif
