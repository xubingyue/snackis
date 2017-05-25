#ifndef SNACKIS_UI_LOGIN_FORM_HPP
#define SNACKIS_UI_LOGIN_FORM_HPP

#include <memory>

#include "ui/form.hpp"
#include "ui/view.hpp"

namespace ui {
  struct LoginForm: public Form {
    Field pass_fld;
    std::unique_ptr<Field> repeat_fld;
    LoginForm(View &view);
  };

  void run(LoginForm &frm);
}

#endif
