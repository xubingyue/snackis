#include "snackis/ctx.hpp"
#include "snackis/db/ctx.hpp"
#include "snackis/ui/decrypt_form.hpp"
#include "snackis/ui/view.hpp"
#include "snackis/ui/window.hpp"

namespace snackis {
namespace ui {
  DecryptForm::DecryptForm(View &view, Footer &ftr):
    ViewForm(view, ftr),
    peer(*this, "Peer"),
    load_from(*this, Dim(1, 50), "Load From"),
    decode(*this, Dim(1, 5), "Decode"),
    save_to(*this, Dim(1, 50), "Save To") {
    label = "Decrypt";
    status = "Press Ctrl-s to decrypt, or Ctrl-q to cancel";
    margin_top = 1;
    
    load_from.margin_top = 1;
    init(load_from, *get_val(ctx.settings.load_folder));
    load_from.on_select = [this]() {
      set_str(save_to, load_from.selected->lbl);
    };

    save_to.margin_top = 1;
    insert(decode, "yes", true);
    insert(decode, "no", false);
  }

  bool run(DecryptForm &frm) {
    Ctx &ctx(frm.window.ctx);
    select(frm.decode, false);
    db::Trans trans(ctx);
    
    while (true) {
      chtype ch = get_key(frm.window);
      
      if (ch == KEY_CTRL('s') || (active_field(frm).ptr == frm.save_to.ptr &&
				  ch == KEY_RETURN)) {
	validate(frm);
	const str save_to(get_str(frm.save_to));

	if (frm.peer.selected && frm.load_from.selected && save_to != "") {
	  const str
	    in_path(frm.load_from.selected->val),
	    out_path(Path(*get_val(ctx.settings.save_folder)) / save_to);
	    
	  log(ctx, fmt("Decrypting from '%0' to '%1'...", in_path, out_path));
	  db::Rec<Peer> peer_rec;
	  set(peer_rec, ctx.db.peer_id, *frm.peer.selected);
	  load(ctx.db.peers, peer_rec);
	  Peer peer(ctx.db.peers, peer_rec);
	  decrypt(peer, in_path, out_path, frm.decode.selected->val);	    
	  db::commit(trans);
	  log(ctx, "Done decrypting");
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
