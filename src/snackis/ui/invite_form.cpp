#include "snackis/ctx.hpp"
#include "snackis/db/ctx.hpp"
#include "snackis/ui/invite_form.hpp"
#include "snackis/ui/view.hpp"
#include "snackis/ui/window.hpp"

namespace snackis {
namespace ui {
  InviteForm::InviteForm(View &view, Footer &ftr):
    ViewForm(view, ftr),
    email(*this, Dim(1, 50), "Email") {
    label = "Invite";
    status = "Press Ctrl-s to send invite, or Ctrl-q to cancel";
    margin_top = 1;
  }

  bool run(InviteForm &frm) {
    Ctx &ctx(frm.window.ctx);
    db::Trans trans(ctx);
    
    while (true) {
      chtype ch = get_key(frm.window);
      
      if (ch == KEY_CTRL('s') || ch == KEY_RETURN) {
	validate(frm);
	Invite inv(ctx, get_str(frm.email));
	load(ctx.db.invites, inv);
	post(inv);	
	log(ctx, "New invite posted to outbox");
	db::commit(trans);
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
