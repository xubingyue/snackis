#include "snackis/ctx.hpp"
#include "snackis/core/fmt.hpp"
#include "snackis/core/path.hpp"
#include "snackis/core/proc.hpp"
#include "snackis/db/ctx.hpp"
#include "snackis/net/imap.hpp"
#include "snackis/net/smtp.hpp"

#include "ui/profile_form.hpp"
#include "ui/view.hpp"
#include "ui/window.hpp"

namespace ui {
  static void test_editor(Ctx &ctx, const str &path) {
    log(ctx, fmt("Launching editor: %0", path));
    int ret(run_proc(path, {"test.txt"}));
    if (ret) { log(ctx, fmt("Editor exited with code %0", ret)); }
    ui::redraw();
  }

  static bool copy_imap(ProfileForm &frm) {
    const str
      url = get_str(frm.imap_url),
      port = get_str(frm.imap_port),
      user = get_str(frm.imap_user),
      pass = get_str(frm.imap_pass);

    set_val(frm.ctx.settings.imap_url, url);
    set_val(frm.ctx.settings.imap_port, to_int64(port));
    set_val(frm.ctx.settings.imap_user, user);
    set_val(frm.ctx.settings.imap_pass, pass);
    
    return !url.empty() && !port.empty() && !user.empty() && !pass.empty();
  }

  static void test_imap(ProfileForm &frm) {
    if (copy_imap(frm)) {
      try {
	Imap imap(frm.ctx);
	log(frm.ctx, "OK");
      } catch (const ImapError &e) {
	log(frm.ctx, e.what());
      }
    }
  }

  static bool copy_smtp(ProfileForm &frm) {
    const str
      url = get_str(frm.smtp_url),
      port = get_str(frm.smtp_port),
      user = get_str(frm.smtp_user),
      pass = get_str(frm.smtp_pass);

    set_val(frm.ctx.settings.smtp_url, url);
    set_val(frm.ctx.settings.smtp_port, to_int64(port));
    set_val(frm.ctx.settings.smtp_user, user);
    set_val(frm.ctx.settings.smtp_pass, pass);
    
    return !url.empty() && !port.empty() && !user.empty() && !pass.empty();
  }

  static void test_smtp(ProfileForm &frm) {
    if (copy_smtp(frm)) {
      try {
	Smtp smtp(frm.ctx);
	log(frm.ctx, "OK");
      } catch (const SmtpError &e) {
	log(frm.ctx, e.what());
      }
    }
  }
  
  ProfileForm::ProfileForm(View &view, Footer &ftr):
    ViewForm(view, ftr),
    
    name(*this, Dim(1, 50), "Name: "),
    email(*this, Dim(1, 50), "Email: "),
    editor(*this, Dim(1, 50), "Editor: "),

    imap_url(*this, Dim(1, 50), "Imap Url: "),
    imap_port(*this, Dim(1, 10), "Imap Port: "),
    imap_user(*this, Dim(1, 50), "Imap User: "),
    imap_pass(*this, Dim(1, 50), "Imap Pass: "),

    smtp_url(*this, Dim(1, 50), "Smtp Url: "),
    smtp_port(*this, Dim(1, 10), "Smtp Port: "),
    smtp_user(*this, Dim(1, 50), "Smtp User: "),
    smtp_pass(*this, Dim(1, 50), "Smtp Pass: ") {
    label = "Profile";
    status = "Press Ctrl-s to save profile, or Ctrl-q to cancel";
    margin_top = 1;
    
    editor.action = [this]() { test_editor(ctx, get_str(editor)); };
    
    auto imap_action([this]() { test_imap(*this); });
    imap_url.action = imap_action;
    imap_port.action = imap_action;
    imap_user.action = imap_action;
    imap_pass.action = imap_action;

    imap_url.margin_top = 1;
    imap_pass.echo = false;

    auto smtp_action([this]() { test_smtp(*this); });
    smtp_url.action = smtp_action;
    smtp_port.action = smtp_action;
    smtp_user.action = smtp_action;
    smtp_pass.action = smtp_action;
    
    smtp_url.margin_top = 1;
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
    
    auto imap_url(get_val(ctx.settings.imap_url));
    if (imap_url) { set_str(frm.imap_url, *imap_url); }
    auto imap_port(get_val(ctx.settings.imap_port));
    if (imap_port) { set_str(frm.imap_port, to_str(*imap_port)); }
    auto imap_user(get_val(ctx.settings.imap_user));
    if (imap_user) { set_str(frm.imap_user, *imap_user); }
    auto imap_pass(get_val(ctx.settings.imap_pass));
    if (imap_pass) { set_str(frm.imap_pass, *imap_pass); }

    auto smtp_url(get_val(ctx.settings.smtp_url));
    if (smtp_url) { set_str(frm.smtp_url, *smtp_url); }
    auto smtp_port(get_val(ctx.settings.smtp_port));
    if (smtp_port) { set_str(frm.smtp_port, to_str(*smtp_port)); }
    auto smtp_user(get_val(ctx.settings.smtp_user));
    if (smtp_user) { set_str(frm.smtp_user, *smtp_user); }
    auto smtp_pass(get_val(ctx.settings.smtp_pass));
    if (smtp_pass) { set_str(frm.smtp_pass, *smtp_pass); }
    
    while (true) {
      chtype ch = get_key(frm.window);
      
      if (ch == KEY_CTRL('s') ||
	  (ch == KEY_RETURN && &active_field(frm) == frm.fields.back())) {
	validate(frm);

	me.name = get_str(frm.name);
	me.email = get_str(frm.email);
	if (!update(ctx.db.peers, me)) { ERROR(db::Db, "Failed updating me"); }
	set_val(ctx.settings.editor, get_str(frm.editor));

	copy_imap(frm);
	copy_smtp(frm);
	db::commit(trans);
	log(frm.window.ctx, "Saved profile");
	return true;
      }

      switch (ch) {
      case KEY_CTRL('q'):
	return false;
      default:
	drive(frm, ch);
      }
    }
  }
}
