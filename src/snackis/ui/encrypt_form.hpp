#ifndef SNACKIS_UI_ENCRYPT_FORM_HPP
#define SNACKIS_UI_ENCRYPT_FORM_HPP

#include "snackis/ui/view_form.hpp"
#include "snackis/ui/enum_field.hpp"
#include "snackis/ui/file_field.hpp"
#include "snackis/ui/text_field.hpp"

namespace snackis {
namespace ui {
  struct EncryptForm: public ViewForm {
    EnumField<UId> peer_name, peer_email;
    FileField load_from;
    TextField save_to;
    EnumField<bool> encode;
    EncryptForm(View &view, Footer &ftr);
  };

  bool run(EncryptForm &frm);
}}

#endif
