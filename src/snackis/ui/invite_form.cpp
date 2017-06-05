#include "snackis/ctx.hpp"
#include "snackis/db/ctx.hpp"
#include "snackis/ui/invite_form.hpp"
#include "snackis/ui/view.hpp"
#include "snackis/ui/window.hpp"

namespace snackis {
namespace ui {
  InviteForm::InviteForm(View &view, Footer &ftr):
    ViewForm(view, ftr),
    email(*this, Dim(1, 50), "Email: "),
    name(*this, Dim(1, 50), "Name: "),
    last_invite(*this, Dim(1, 50), "Last Invite: "),
    send_now(*this, Dim(1, 3), "Send Now: ") {
    label = "Invite";
    status = "Press Ctrl-s to send invite, or Ctrl-q to cancel";
    margin_top = 1;

    last_invite.margin_top = 1;
    last_invite.active = false;
    send_now.margin_top = 1;
  }

  bool run(InviteForm &frm) {
    Ctx &ctx(frm.window.ctx);
    db::Trans trans(ctx);
    
    while (true) {
      chtype ch = get_key(frm.window);
      
      if (ch == KEY_CTRL('s') ||
	  (ch == KEY_RETURN && &active_field(frm) == frm.fields.back())) {
	validate(frm);
	db::commit(trans);
	log(frm.window.ctx, "Saved invite to outbox");
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
