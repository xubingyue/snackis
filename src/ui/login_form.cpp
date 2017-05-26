#include "snackis/ctx.hpp"
#include "snackis/core/path.hpp"
#include "snackis/db/ctx.hpp"

#include "ui/login_form.hpp"
#include "ui/view.hpp"

namespace ui {
  LoginForm::LoginForm(View &view):
    Form(view),
    view(view),
    pass_fld(*this, Dim(1, 50), "Password: ") {
    pass_fld.echo = false;
    
    if (!pass_exists(window.ctx)) {
      repeat_fld.reset(new Field(*this, Dim(1, 50), "Repeat: "));
      repeat_fld->echo = false;
    }
    
    margin_top = 1;
  }

  bool run(LoginForm &frm) {
    set_label(frm.view, "Login");
    
    if (frm.repeat_fld) {
      log(frm.window.ctx, "Please select a system password and type it twice");
    } else {
      log(frm.window.ctx, "Please enter system password");
    }
    
    log(frm.window.ctx, "Press Escape to exit");
    
    while (true) {
      chtype ch = get_key(frm.window);

      if (ch == KEY_ESCAPE) { return false; }

      if (ch == KEY_RETURN && &active_field(frm) == frm.fields.back()) {
	validate(frm);
	
	if (frm.repeat_fld) {
	  if (get_str(*frm.repeat_fld) != get_str(frm.pass_fld)) {
	    log(frm.window.ctx, "Password mismatch");
	    continue;
	  }

	  db::init_pass(frm.window.ctx,  get_str(frm.pass_fld));
	} else {
	  if (!db::login(frm.window.ctx, get_str(frm.pass_fld))) {
	    log(frm.window.ctx, "Password mismatch");
	    continue;
	  }	  
	}
	
	break;	  
      }
      
      drive(frm, ch);
    }

    set_label(frm.view, "");
    return true;
  }
}
