#ifndef SNACKIS_UI_PROFILE_FORM_HPP
#define SNACKIS_UI_PROFILE_FORM_HPP

#include "ui/form.hpp"

namespace ui {
  struct View;
  
  struct ProfileForm: public Form {
    View &view;
    Field
    name, email, editor,
      imap_server, imap_port, imap_user, imap_pass,
      smtp_server, smtp_port, smtp_user, smtp_pass;
    
    ProfileForm(View &view);
  };

  void run(ProfileForm &frm);
}

#endif
