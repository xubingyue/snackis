#ifndef SNACKIS_UI_PROFILE_FORM_HPP
#define SNACKIS_UI_PROFILE_FORM_HPP

#include "ui/form.hpp"

namespace ui {
  struct View;
  
  struct ProfileForm: public Form {
    View &view;
    Field name_fld, email_fld;
    
    ProfileForm(View &view);
  };

  void run(ProfileForm &frm);
}

#endif
