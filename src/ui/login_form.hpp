#ifndef SNACKIS_UI_LOGIN_FORM_HPP
#define SNACKIS_UI_LOGIN_FORM_HPP

#include <memory>

#include "ui/form.hpp"

namespace ui {
  struct View;
  
  struct LoginForm: public Form {
    View &view;
    Field pass_fld;
    std::unique_ptr<Field> repeat_fld;
    
    LoginForm(View &view);
  };

  bool run(LoginForm &frm);
}

#endif
