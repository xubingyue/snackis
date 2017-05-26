#include "snackis/ctx.hpp"
#include "snackis/core/path.hpp"
#include "snackis/db/ctx.hpp"

#include "ui/profile_form.hpp"
#include "ui/view.hpp"

namespace ui {
  ProfileForm::ProfileForm(View &view):
    Form(view),
    view(view),
    name_fld(*this, Dim(1, 50), "Name: "),
    email_fld(*this, Dim(1, 50), "Email: ") {
    margin_top = 1;
  }

  void run(ProfileForm &frm) {
    set_label(frm.view, "Profile");
    bool done = false;
    
    while (!done) {
      chtype ch = get_key(frm.window);

      if (ch == KEY_RETURN && &active_field(frm) == frm.fields.back()) {
	validate(frm);
	done = true;	  
      }
      
      drive(frm, ch);
    }

    set_label(frm.view, "");
  }
}
