#include "snackis/ctx.hpp"
#include "snackis/db/ctx.hpp"
#include "snackis/ui/encrypt_form.hpp"
#include "snackis/ui/view.hpp"
#include "snackis/ui/window.hpp"

namespace snackis {
namespace ui {
  EncryptForm::EncryptForm(View &view, Footer &ftr):
    ViewForm(view, ftr),
    peer(*this, "Peer"),
    load_from(*this, Dim(1, 50), "Load From"),
    save_to(*this, Dim(1, 50), "Save To"),
    encode(*this, Dim(1, 5), "Encode") {
    label = "Encrypt";
    status = "Press Ctrl-s to encrypt, or Ctrl-q to cancel";
    margin_top = 1;
    
    load_from.margin_top = 1;
    init(load_from, *get_val(ctx.settings.load_folder));
    load_from.on_change = [this]() {
      set_str(save_to, load_from.selected->lbl);
    };

    save_to.margin_top = 1;
    insert(encode, "Yes", true);
    insert(encode, "No", false);
  }

  bool run(EncryptForm &frm) {
    Ctx &ctx(frm.window.ctx);
    select(frm.encode, false);
    db::Trans trans(ctx);
    
    while (true) {
      chtype ch = get_key(frm.window);
      
      if (ch == KEY_CTRL('s') || (active_field(frm).ptr == frm.encode.ptr &&
				  ch == KEY_RETURN)) {
	validate(frm);
	const str save_to(get_str(frm.save_to));

	if (frm.peer.selected && frm.load_from.selected && save_to != "") {
	  const str
	    in_path(frm.load_from.selected->val),
	    out_path(Path(*get_val(ctx.settings.save_folder)) / save_to);
	    
	  log(ctx, fmt("Encrypting from '%0' to '%1'...", in_path, out_path));
	  db::Rec<Peer> peer_rec;
	  set(peer_rec, ctx.db.peer_id, *frm.peer.selected);
	  load(ctx.db.peers, peer_rec);
	  Peer peer(ctx.db.peers, peer_rec);
	  encrypt(peer, in_path, out_path, frm.encode.selected->val);	    
	  db::commit(trans);
	  log(ctx, "Done encrypting");
	  break;
	}
      }

      switch (ch) {
      case KEY_CTRL('q'):
	return false;
      default:
	drive(frm, ch);
      }
    }

    return true;
  }
}}
