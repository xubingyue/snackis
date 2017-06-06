#include "snackis/ctx.hpp"
#include "snackis/ui/enum_field.hpp"
#include "snackis/ui/inbox_form.hpp"
#include "snackis/ui/view.hpp"
#include "snackis/ui/window.hpp"

namespace snackis {
namespace ui {
  enum InviteResponse {INVITE_ACCEPT, INVITE_REJECT};

  InboxForm::InboxForm(View &view, Footer &ftr):
    ViewForm(view, ftr) {
    label = "Inbox";
    status = "Press Ctrl-s to perform selected actions, or Ctrl-q to cancel";
    margin_top = 1;

    for (auto rec: ctx.db.inbox.recs) {
      const Msg msg(ctx.db.inbox, rec);
      if (msg.type == Msg::INVITE) {
	auto fld(new EnumField<InviteResponse>(*this, Dim(1, 10),
					       fmt("Invite from %0 (%1)",
						   msg.peer_name, msg.peer_email)));
	fld->allow_clear = true;
	push(*fld, "accept", INVITE_ACCEPT);
	push(*fld, "reject", INVITE_REJECT);
	field_lookup[msg.id] = fld;
      } else {
	erase(ctx.db.inbox, msg);
	log(ctx, fmt("Removed message %0 due to invalid type: %1",
		     msg.id, msg.type));
      }
    }
  }

  bool run(InboxForm &frm) {
    Ctx &ctx(frm.window.ctx);
    db::Trans trans(ctx);
    
    while (true) {
      chtype ch = get_key(frm.window);
      
      if (ch == KEY_CTRL('s') ||
	  (ch == KEY_RETURN && &active_field(frm) == frm.fields.back())) {
	validate(frm);

	while (!ctx.db.inbox.recs.empty()) {
	  db::Rec<Msg> rec(*ctx.db.inbox.recs.begin());
	  const Msg msg(ctx.db.inbox, rec);
	  auto fld(frm.field_lookup[msg.id]);

	  if (msg.type == Msg::INVITE) {
	    auto resp_fld(dynamic_cast<EnumField<InviteResponse> *>(fld));
	    if (resp_fld->selected) {
	      switch(resp_fld->selected->val) {
	      case INVITE_ACCEPT:
		break;
	      case INVITE_REJECT:
		break;
	      }
	    }
	  }

	  erase(ctx.db.inbox, rec);
	}
	
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
