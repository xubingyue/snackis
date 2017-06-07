#ifndef SNACKIS_UI_DECRYPT_FORM_HPP
#define SNACKIS_UI_DECRYPT_FORM_HPP

#include "snackis/ui/view_form.hpp"
#include "snackis/ui/enum_field.hpp"
#include "snackis/ui/file_field.hpp"
#include "snackis/ui/peer_panel.hpp"
#include "snackis/ui/text_field.hpp"

namespace snackis {
namespace ui {
  struct DecryptForm: public ViewForm {
    PeerPanel peer;
    FileField load_from;
    EnumField<bool> decode;
    TextField save_to;
    DecryptForm(View &view, Footer &ftr);
  };

  bool run(DecryptForm &frm);
}}

#endif
