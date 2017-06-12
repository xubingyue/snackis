#include "snackis/ctx.hpp"
#include "snackis/core/fmt.hpp"
#include "snackis/core/path.hpp"
#include "snackis/core/proc.hpp"
#include "snackis/db/ctx.hpp"
#include "snackis/net/imap.hpp"
#include "snackis/net/smtp.hpp"
#include "snackis/ui/settings_form.hpp"
#include "snackis/ui/view.hpp"
#include "snackis/ui/window.hpp"

namespace snackis {
namespace ui {
  static bool copy_imap(SettingsForm &frm) {
    Ctx &ctx(frm.ctx);
    
    const str
      url = get_str(frm.imap_url),
      user = get_str(frm.imap_user),
      pass = get_str(frm.imap_pass);

    set_val(ctx.settings.imap_url, url);
    set_val(ctx.settings.imap_user, user);
    set_val(ctx.settings.imap_pass, pass);
    
    opt<int64_t>
      port(to_int64(get_str(frm.imap_port))),
      poll(to_int64(get_str(frm.imap_poll)));
    if (port) { set_val(ctx.settings.imap_port, *port); }

    if (poll) {
      set_val(ctx.settings.imap_poll, *poll);
      ctx.fetch_cond.notify_one();
    }

    return !url.empty() && !port && !user.empty() && !pass.empty();
  }

  static void test_imap(SettingsForm &frm) {
    if (copy_imap(frm)) {
      try {
	Imap imap(frm.ctx);
	log(frm.ctx, "Ok");
      } catch (const ImapError &e) {
	log(frm.ctx, e.what());
      }
    }
  }

  static bool copy_smtp(SettingsForm &frm) {
    Ctx &ctx(frm.ctx);

    const str
      url = get_str(frm.smtp_url),
      user = get_str(frm.smtp_user),
      pass = get_str(frm.smtp_pass);

    set_val(ctx.settings.smtp_url, url);
    set_val(ctx.settings.smtp_user, user);
    set_val(ctx.settings.smtp_pass, pass);

    opt<int64_t>
      port = to_int64(get_str(frm.smtp_port)),
      poll = to_int64(get_str(frm.smtp_poll));

    if (port) { set_val(ctx.settings.smtp_port, *port); }
    
    if (poll) {
      set_val(ctx.settings.smtp_poll, *poll);
      ctx.send_cond.notify_one();
    }

    return !url.empty() && !port && !user.empty() && !pass.empty();
  }

  static void test_smtp(SettingsForm &frm) {
    if (copy_smtp(frm)) {
      try {
	Smtp smtp(frm.ctx);
	log(frm.ctx, "Ok");
      } catch (const SmtpError &e) {
	log(frm.ctx, e.what());
      }
    }
  }
  
  SettingsForm::SettingsForm(View &view, Footer &ftr):
    ViewForm(view, ftr),
    
    name(*this, Dim(1, 50), "Name"),
    email(*this, Dim(1, 50), "Email"),

    load_folder(*this, Dim(1, 50), "Load-folder"),
    save_folder(*this, Dim(1, 50), "Save-folder"),

    imap_url(*this, Dim(1, 50), "Imap Url"),
    imap_port(*this, Dim(1, 10), "Imap Port"),
    imap_user(*this, Dim(1, 50), "Imap User"),
    imap_pass(*this, Dim(1, 50), "Imap Password"),
    imap_poll(*this, Dim(1, 5), "Imap Poll (s)"),

    smtp_url(*this, Dim(1, 50), "Smtp Url"),
    smtp_port(*this, Dim(1, 10), "Smtp Port"),
    smtp_user(*this, Dim(1, 50), "Smtp User"),
    smtp_pass(*this, Dim(1, 50), "Smtp Password"),
    smtp_poll(*this, Dim(1, 5), "Smtp Poll (s)") {
    label = "Settings";
    status = "Press Ctrl-s to save settings, or Ctrl-q to cancel";
    margin_top = 1;
    load_folder.margin_top = 1;
    
    imap_url.margin_top = 1;
    imap_pass.echo = false;
    auto imap_action([this]() { test_imap(*this); });
    imap_url.on_action = imap_action;
    imap_port.on_action = imap_action;
    imap_user.on_action = imap_action;
    imap_pass.on_action = imap_action;
    imap_pass.info = "Press Ctrl-Space to try connecting";
    imap_poll.margin_top = 1;
    imap_poll.on_action = imap_action;

    smtp_url.margin_top = 2;
    smtp_pass.echo = false;
    auto smtp_action([this]() { test_smtp(*this); });
    smtp_url.on_action = smtp_action;
    smtp_port.on_action = smtp_action;
    smtp_user.on_action = smtp_action;
    smtp_pass.on_action = smtp_action;    
    smtp_pass.info = "Press Ctrl-Space to try connecting";
    smtp_poll.margin_top = 1;
    smtp_poll.on_action = imap_action;
  }

  bool run(SettingsForm &frm) {
    Ctx &ctx(frm.window.ctx);
    db::Trans trans(ctx);
    
    Peer &me(whoami(ctx));
    set_str(frm.name, me.name);
    set_str(frm.email, me.email);
    
    set_str(frm.load_folder, *get_val(ctx.settings.load_folder));
    set_str(frm.save_folder, *get_val(ctx.settings.save_folder));
    
    set_str(frm.imap_url, *get_val(ctx.settings.imap_url));
    set_str(frm.imap_port, to_str(*get_val(ctx.settings.imap_port)));
    auto imap_user(get_val(ctx.settings.imap_user));
    if (imap_user) { set_str(frm.imap_user, *imap_user); }
    auto imap_pass(get_val(ctx.settings.imap_pass));
    if (imap_pass) { set_str(frm.imap_pass, *imap_pass); }
    auto imap_poll(get_val(ctx.settings.imap_poll));
    if (imap_poll) { set_str(frm.imap_poll, to_str(*imap_poll)); }

    set_str(frm.smtp_url, *get_val(ctx.settings.smtp_url));
    set_str(frm.smtp_port, to_str(*get_val(ctx.settings.smtp_port)));
    auto smtp_user(get_val(ctx.settings.smtp_user));
    if (smtp_user) { set_str(frm.smtp_user, *smtp_user); }
    auto smtp_pass(get_val(ctx.settings.smtp_pass));
    if (smtp_pass) { set_str(frm.smtp_pass, *smtp_pass); }
    auto smtp_poll(get_val(ctx.settings.smtp_poll));
    if (smtp_poll) { set_str(frm.smtp_poll, to_str(*smtp_poll)); }
    
    while (true) {
      chtype ch = get_key(frm.window);
      
      if (ch == KEY_CTRL('s')) {
	validate(frm);

	me.name = get_str(frm.name);
	me.email = get_str(frm.email);
	if (!update(ctx.db.peers, me)) { ERROR(Db, "Failed updating me"); }
	set_val(ctx.settings.load_folder, get_str(frm.load_folder));
	set_val(ctx.settings.save_folder, get_str(frm.save_folder));
	copy_imap(frm);
	copy_smtp(frm);
	
	db::commit(trans);
	log(frm.window.ctx, "Saved settings");
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
}}
