#include "snackis/ctx.hpp"
#include "ui/login_form.hpp"

namespace ui {
  LoginForm::LoginForm(View &view):
    Form(view),
    pass_fld(*this, Dim(1, 50), "Password: ") {
    repeat_fld.reset(new Field(*this, Dim(1, 50), "Repeat: "));
    margin_top = 1;
  }

  void run(LoginForm &frm) {
    bool done = false;
    while (!done) {
      chtype ch = get_key(frm.window);
      if (ch == KEY_RETURN) { done = true; }
      drive(frm, ch);
    }
  }
}
