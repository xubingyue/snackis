#include "snackis/ctx.hpp"
#include "snackis/ui/peer_panel.hpp"

namespace snackis {
namespace ui {
  PeerPanel::PeerPanel(Form &frm, const str &prefix):
    name(frm, Dim(1, 50), fmt("%0 Name", prefix)),
    email(frm, Dim(1, 50), fmt("%0 Email", prefix)) {
    Ctx &ctx(frm.ctx);
    name.allow_clear = true;
    email.allow_clear = true;
    
    for (auto p: ctx.db.peers.recs) {
      auto id(*get(p, ctx.db.peer_id));
      insert(name, *get(p, ctx.db.peer_name), id);
      insert(email, *get(p, ctx.db.peer_email), id);
    }

    name.on_change = [this]() {
      if (name.selected) {
	auto &sel(name.selected);
	assert(sel);
	select(email, sel->val, false);
	selected = sel->val;
      } else {
	clear(email, false);
	selected = nullopt;
      }

    };

    email.on_change = [this]() {
      if (email.selected) {
	auto &sel(email.selected);
	assert(sel);
	select(name, sel->val, false);
	selected = sel->val;
      } else {
	clear(name, false);
	selected = nullopt;
      }
    };
  }
}}
