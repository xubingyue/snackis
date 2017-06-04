#ifndef SNACKIS_UI_LOGIN_FORM_HPP
#define SNACKIS_UI_LOGIN_FORM_HPP

#include <memory>

#include "snackis/ui/view_form.hpp"

namespace snackis {
namespace ui {
  struct LoginForm: public ViewForm {
    Field pass;
    std::unique_ptr<Field> repeat;
    
    LoginForm(View &view, Footer &ftr);
  };

  bool run(LoginForm &frm);
}}

#endif
