#include "snackis/ctx.hpp"
#include "snackis/db/ctx.hpp"
#include "snackis/ui/encrypt_form.hpp"
#include "snackis/ui/view.hpp"
#include "snackis/ui/window.hpp"

namespace snackis {
namespace ui {
  EncryptForm::EncryptForm(View &view, Footer &ftr):
    ViewForm(view, ftr),
    peer_name(*this, Dim(1, 50), "Peer Name"),
    peer_email(*this, Dim(1, 50), "Peer Email"),
    load_from(*this, Dim(1, 50), "Load From"),
    save_to(*this, Dim(1, 50), "Save To"),
    encode_result(*this, Dim(1, 5), "Encode Result") {
    label = "Encrypt";
    status = "Press Ctrl-s to encrypt, or Ctrl-q to cancel";
    margin_top = 1;
    
    for (auto p: ctx.db.peers.recs) {
      auto id(*get(p, ctx.db.peer_id));
      push(peer_name, *get(p, ctx.db.peer_name), id);
      push(peer_email, *get(p, ctx.db.peer_email), id);
    }

    load_from.margin_top = 1;
    push(encode_result, "yes", true);
    push(encode_result, "no", false);
  }

  bool run(EncryptForm &frm) {
    Ctx &ctx(frm.window.ctx);

    const str ldf(*get_val(ctx.settings.load_folder));

    if (!path_exists(ldf)) {
      create_path(ldf);
      log(ctx, fmt("Nothing to load"));
      return false;
    }
    
    select(frm.encode_result, 1);
    db::Trans trans(ctx);
    
    while (true) {
      chtype ch = get_key(frm.window);
      
      if (ch == KEY_CTRL('s') || ch == KEY_RETURN) {
	validate(frm);
	create_path(*get_val(ctx.settings.save_folder));
	const str save_to(get_str(frm.save_to));
	//TODO: write encrypted data
	db::commit(trans);
	log(ctx, fmt("Encrypted data saved to: %0", save_to));
	break;
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
