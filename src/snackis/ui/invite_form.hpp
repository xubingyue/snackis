#ifndef SNACKIS_UI_INVITE_FORM_HPP
#define SNACKIS_UI_INVITE_FORM_HPP

#include "snackis/ui/view_form.hpp"
#include "snackis/ui/enum_field.hpp"
#include "snackis/ui/text_field.hpp"

namespace snackis {
namespace ui {
  struct InviteForm: public ViewForm {
    TextField email, name, last_invite;
    EnumField<bool> send_now;
    InviteForm(View &view, Footer &ftr);
  };

  bool run(InviteForm &frm);
}}

#endif
