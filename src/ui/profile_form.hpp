#ifndef SNACKIS_UI_PROFILE_FORM_HPP
#define SNACKIS_UI_PROFILE_FORM_HPP

#include "ui/view_form.hpp"

namespace ui {
  struct ProfileForm: public ViewForm {
    Field
    name, email, editor,
      imap_server, imap_port, imap_user, imap_pass,
      smtp_server, smtp_port, smtp_user, smtp_pass;
    
    ProfileForm(View &view, Footer &ftr);
  };

  bool run(ProfileForm &frm);
}

#endif
