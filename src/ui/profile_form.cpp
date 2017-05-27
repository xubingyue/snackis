#include "snackis/ctx.hpp"
#include "snackis/core/path.hpp"
#include "snackis/db/ctx.hpp"

#include "ui/profile_form.hpp"
#include "ui/view.hpp"

namespace ui {
  ProfileForm::ProfileForm(View &view):
    Form(view),
    view(view),
    name(*this, Dim(1, 50), "Name: "),
    email(*this, Dim(1, 50), "Email: "),
    editor(*this, Dim(1, 50), "Editor: "),

    imap_server(*this, Dim(1, 50), "Imap Server: "),
    imap_port(*this, Dim(1, 10), "Imap Port: "),
    imap_user(*this, Dim(1, 50), "Imap User: "),
    imap_pass(*this, Dim(1, 50), "Imap Pass: "),

    smtp_server(*this, Dim(1, 50), "Smtp Server: "),
    smtp_port(*this, Dim(1, 10), "Smtp Port: "),
    smtp_user(*this, Dim(1, 50), "Smtp User: "),
    smtp_pass(*this, Dim(1, 50), "Smtp Pass: ") {
    margin_top = 1;
    imap_server.margin_top = 1;
    imap_pass.echo = false;
    smtp_server.margin_top = 1;
    smtp_pass.echo = false;
  }

  void run(ProfileForm &frm) {
    set_label(frm.view, "Profile");
    log(frm.window.ctx, "Press Ctrl-s to save profile");

    while (true) {
      chtype ch = get_key(frm.window);

      if (ch == CTRL('q')) { break; }
      
      if (ch == CTRL('s') ||
	  (ch == KEY_RETURN && &active_field(frm) == frm.fields.back())) {
	validate(frm);
	log(frm.window.ctx, "Saved profile");
	break;
      }
      
      drive(frm, ch);
    }

    set_label(frm.view, "");
  }
}
