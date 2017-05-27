#include "snackis/ctx.hpp"
#include "snackis/core/path.hpp"
#include "snackis/db/ctx.hpp"

#include "ui/login_form.hpp"
#include "ui/window.hpp"

namespace ui {
  LoginForm::LoginForm(View &view, Footer &ftr):
    ViewForm(view, ftr),
    pass(*this, Dim(1, 50), "Password: ") {
    label = "Login";
    status = "Press Ctrl-q to cancel and exit";

    pass.echo = false;
    
    if (!pass_exists(window.ctx)) {
      repeat.reset(new Field(*this, Dim(1, 50), "Repeat: "));
      repeat->echo = false;
    }
    
    margin_top = 1;
  }

  bool run(LoginForm &frm) {
    if (frm.repeat) {
      log(frm.window.ctx, "Please select a system password and type it twice");
    } else {
      log(frm.window.ctx, "Please enter system password");
    }
        
    while (true) {
      chtype ch = get_key(frm.window);

      if (ch == CTRL('q')) { return false; }

      if (ch == KEY_RETURN && &active_field(frm) == frm.fields.back()) {
	validate(frm);
	
	if (frm.repeat) {
	  if (get_str(*frm.repeat) != get_str(frm.pass)) {
	    log(frm.window.ctx, "Password mismatch");
	    continue;
	  }

	  db::init_pass(frm.window.ctx,  get_str(frm.pass));
	} else {
	  if (!db::login(frm.window.ctx, get_str(frm.pass))) {
	    log(frm.window.ctx, "Password mismatch");
	    continue;
	  }	  
	}
	
	break;	  
      }
      
      drive(frm, ch);
    }

    return true;
  }
}
