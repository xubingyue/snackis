#ifndef SNACKIS_UI_LOGIN_FORM_HPP
#define SNACKIS_UI_LOGIN_FORM_HPP

#include <memory>

#include "snackis/ui/view_form.hpp"
#include "snackis/ui/text_field.hpp"

namespace snackis {
namespace ui {
  struct LoginForm: public ViewForm {
    TextField pass;
    std::unique_ptr<TextField> repeat;
    
    LoginForm(View &view, Footer &ftr);
  };

  bool run(LoginForm &frm);
}}

#endif
