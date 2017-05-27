#include "snackis/ctx.hpp"
#include "snackis/core/path.hpp"
#include "snackis/db/ctx.hpp"

#include "ui/profile_form.hpp"
#include "ui/window.hpp"

namespace ui {
  ProfileForm::ProfileForm(View &view, Footer &ftr):
    ViewForm(view, ftr),
    
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
    label = "Profile";
    status = "Press Ctrl-s to save profile, or Ctrl-q to cancel";
    
    margin_top = 1;
    imap_server.margin_top = 1;
    imap_pass.echo = false;
    smtp_server.margin_top = 1;
    smtp_pass.echo = false;
  }

  bool run(ProfileForm &frm) {
    Ctx &ctx(frm.window.ctx);
    db::Trans trans(ctx);
    
    Peer &me(whoami(ctx));
    set_str(frm.name, me.name);
    set_str(frm.email, me.email);
    
    auto editor(get_val(ctx.settings.editor));
    if (editor) { set_str(frm.editor, *editor); }
    
    auto imap_server(get_val(ctx.settings.imap_server));
    if (imap_server) { set_str(frm.imap_server, *imap_server); }
    auto imap_user(get_val(ctx.settings.imap_user));
    if (imap_user) { set_str(frm.imap_user, *imap_user); }
    auto imap_pass(get_val(ctx.settings.imap_pass));
    if (imap_pass) { set_str(frm.imap_pass, *imap_pass); }

    auto smtp_server(get_val(ctx.settings.smtp_server));
    if (smtp_server) { set_str(frm.smtp_server, *smtp_server); }
    auto smtp_user(get_val(ctx.settings.smtp_user));
    if (smtp_user) { set_str(frm.smtp_user, *smtp_user); }
    auto smtp_pass(get_val(ctx.settings.smtp_pass));
    if (smtp_pass) { set_str(frm.smtp_pass, *smtp_pass); }
    
    while (true) {
      chtype ch = get_key(frm.window);

      if (ch == CTRL('q')) { return false; }
      
      if (ch == CTRL('s') ||
	  (ch == KEY_RETURN && &active_field(frm) == frm.fields.back())) {
	validate(frm);
	db::commit(trans);
	log(frm.window.ctx, "Saved profile");
	return true;
      }
      
      drive(frm, ch);
    }
  }
}
