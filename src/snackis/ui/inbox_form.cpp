#include "snackis/ctx.hpp"
#include "snackis/ui/inbox_form.hpp"
#include "snackis/ui/view.hpp"
#include "snackis/ui/window.hpp"

namespace snackis {
namespace ui {
  InboxForm::InboxForm(View &view, Footer &ftr):
    ViewForm(view, ftr) {
    label = "Inbox";
    status = "Press Ctrl-s to perform selected actions, or Ctrl-q to cancel";
    margin_top = 1;
  }

  bool run(InboxForm &frm) {
    Ctx &ctx(frm.window.ctx);
    db::Trans trans(ctx);
    
    while (true) {
      chtype ch = get_key(frm.window);
      
      if (ch == KEY_CTRL('s') ||
	  (ch == KEY_RETURN && &active_field(frm) == frm.fields.back())) {
	validate(frm);
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
