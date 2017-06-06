#include "snackis/ctx.hpp"
#include "snackis/ui/enum_field.hpp"
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
    db::Trans trans(ctx);
    std::set<db::Rec<Msg>> temp_recs(ctx.db.inbox.recs.begin(),
				     ctx.db.inbox.recs.end());
    
    for (auto msg_rec: temp_recs) {
      const Msg msg(ctx.db.inbox, msg_rec);
      if (msg.type == Msg::INVITE) {
	auto fld(new EnumField<bool>(*this, Dim(1, 10),
				     fmt("Invite from %0 (%1)",
					 msg.peer_name, msg.from)));
	fld->allow_clear = true;
	push(*fld, "accept", true);
	push(*fld, "reject", false);
	field_lookup[msg.id] = fld;
      } else if (msg.type == Msg::ACCEPT || msg.type == Msg::REJECT) {
	db::Rec<Invite> inv_rec;
	set(inv_rec, ctx.db.invite_to, msg.from);
	
	if (load(ctx.db.invites, inv_rec)) {
	  auto fld(new EnumField<bool>(*this, Dim(1, 10),
				       fmt("Invite to %0 (%1) was %2",
					   msg.peer_name, msg.from,
					   (msg.type == Msg::ACCEPT)
					   ? "accepted"
					   : "rejected")));
	  fld->allow_clear = true;
	  push(*fld, "ok", true);
	  field_lookup[msg.id] = fld;
	} else {
	  erase(ctx.db.inbox, msg_rec);
	}
      } else {
	ERROR(db::Db, fmt("Invalid message type: %0", msg.type));
      }
    }

    db::commit(trans);
  }

  bool run(InboxForm &frm) {
    Ctx &ctx(frm.window.ctx);
    db::Trans trans(ctx);
    
    while (true) {
      chtype ch = get_key(frm.window);
      
      if (ch == KEY_CTRL('s') ||
	  (ch == KEY_RETURN && &active_field(frm) == frm.fields.back())) {
	validate(frm);

	for (auto i: frm.field_lookup) {
	  db::Rec<Msg> rec;
	  set(rec, ctx.db.msg_id, i.first);
	  load(ctx.db.inbox, rec);
	  const Msg msg(ctx.db.inbox, rec);

	  if (msg.type == Msg::INVITE) {
	    auto resp_fld(dynamic_cast<EnumField<bool> *>(i.second));

	    if (resp_fld->selected) {
	      if (resp_fld->selected->val) {
		accept_invite(msg);
		log(ctx, fmt("Accept of %0 (%1) posted to outbox",
			     msg.peer_name, msg.from));
	      } else {
		reject_invite(msg);
		log(ctx, fmt("Reject of %0 (%1) posted to outbox",
			     msg.peer_name, msg.from));
	      }

	      erase(ctx.db.inbox, rec);
	    }
	  } else if (msg.type == Msg::ACCEPT || msg.type == Msg::REJECT) {
	    db::Rec<Invite> inv_rec;
	    set(inv_rec, ctx.db.invite_to, msg.from);
	    erase(ctx.db.invites, inv_rec);
	    
	    auto resp_fld(dynamic_cast<EnumField<bool> *>(i.second));
	    if (resp_fld->selected) { erase(ctx.db.inbox, rec); }
	  } else {
	    ERROR(db::Db, fmt("Invalid message type: %0", msg.type));
	  }
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
