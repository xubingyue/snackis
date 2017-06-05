#ifndef SNACKIS_UI_INVITE_FORM_HPP
#define SNACKIS_UI_INVITE_FORM_HPP

#include "snackis/ui/view_form.hpp"

namespace snackis {
namespace ui {
  struct InviteForm: public ViewForm {
    Field email, name, last_invite, send_now;
    
    InviteForm(View &view, Footer &ftr);
  };

  bool run(InviteForm &frm);
}}

#endif
