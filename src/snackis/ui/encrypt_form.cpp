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
    encode(*this, Dim(1, 5), "Encode") {
    label = "Encrypt";
    status = "Press Ctrl-s to encrypt, or Ctrl-q to cancel";
    margin_top = 1;
    
    for (auto p: ctx.db.peers.recs) {
      auto id(*get(p, ctx.db.peer_id));
      insert(peer_name, *get(p, ctx.db.peer_name), id);
      insert(peer_email, *get(p, ctx.db.peer_email), id);
    }

    peer_name.on_select = [this]() {
      auto &sel(peer_name.selected);
      assert(sel);
      select(peer_email, sel->val, false);
    };

    peer_email.on_select = [this]() {
      auto &sel(peer_email.selected);
      assert(sel);
      select(peer_name, sel->val, false);
    };

    load_from.margin_top = 1;
    for (auto &i: PathIter(*get_val(ctx.settings.load_folder))) {
      const Path p(i);
      insert(load_from, p.filename(), p);
    }
    
    load_from.on_select = [this]() {
      set_str(save_to, load_from.selected->lbl);
    };

    insert(encode, "yes", true);
    insert(encode, "no", false);
  }

  bool run(EncryptForm &frm) {
    Ctx &ctx(frm.window.ctx);

    const str ldf(*get_val(ctx.settings.load_folder));

    if (!path_exists(ldf)) {
      create_path(ldf);
      log(ctx, fmt("Nothing to load"));
      return false;
    }
    
    select(frm.encode, false);
    db::Trans trans(ctx);
    
    while (true) {
      chtype ch = get_key(frm.window);
      
      if (ch == KEY_CTRL('s') || ch == KEY_RETURN) {
	validate(frm);
	create_path(*get_val(ctx.settings.save_folder));
	auto peer(frm.peer_email.selected);
	auto load_from(frm.load_from.selected);
	const str save_to(get_str(frm.save_to));

	if (peer && load_from && save_to != "") {
	  const str
	    in_path(load_from->val),
	    out_path(Path(*get_val(ctx.settings.save_folder)) / save_to);
	    
	  log(ctx, fmt("Encrypting from '%0' to '%1'", in_path, out_path));

	  std::ifstream in_file;
	  in_file.open(in_path, std::ios::in | std::ios::binary);
	  in_file.seekg(0, std::ios::end);
	  Data in(in_file.tellg(), 0);
	  in_file.seekg(0);
	  in_file.read(reinterpret_cast<char *>(&in[0]), in.size());
	  in_file.close();
	  
	  db::Rec<Peer> peer_rec;
	  set(peer_rec, ctx.db.peer_id, peer->val);
	  load(ctx.db.peers, peer_rec);
	
	  Data out(crypt::encrypt(*get_val(ctx.settings.crypt_key),
				  *get(peer_rec, ctx.db.peer_crypt_key),
				  &in[0], in.size()));
	  
	  std::ofstream out_file;
	  out_file.open(out_path, std::ios::out | std::ios::trunc | std::ios::binary);
	  out_file.write(reinterpret_cast<const char *>(&out[0]), out.size());
	  out_file.close();

	  db::commit(trans);
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
