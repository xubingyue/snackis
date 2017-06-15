#include "snackis/ctx.hpp"
#include "snackis/invite.hpp"
#include "snackis/core/fmt.hpp"
#include "snackis/gui/decrypt.hpp"
#include "snackis/gui/encrypt.hpp"
#include "snackis/gui/gui.hpp"
#include "snackis/gui/inbox.hpp"
#include "snackis/gui/reader.hpp"
#include "snackis/gui/widget.hpp"

namespace snackis {
namespace gui {
  static void init_cmds(Reader &rdr) {
    Ctx &ctx(rdr.ctx);

    rdr.cmds.emplace("clear", [&ctx](auto id, auto args) {
	if (!args.empty()) {
	  log(ctx, "Invalid number of arguments, syntax: clear");
	  return false;
	}
	
	clear(*console);
	return true;
      });

    rdr.cmds.emplace("decrypt", [&ctx](auto id, auto args) {
	if (!args.empty()) {
	  log(ctx, "Invalid number of arguments, syntax: decrypt");
	  return false;
	}
	
	Decrypt *view = new Decrypt(ctx);
	view->push_view();
	return true;
      });

    rdr.cmds.emplace("encrypt", [&ctx](auto id, auto args) {
	if (!args.empty()) {
	  log(ctx, "Invalid number of arguments, syntax: encrypt");
	  return false;
	}
	
	Encrypt *view = new Encrypt(ctx);
	view->push_view();
	return true;
      });

    rdr.cmds.emplace("inbox", [&ctx](auto id, auto args) {
	if (!args.empty()) {
	  log(ctx, "Invalid number of arguments, syntax: inbox");
	  return false;
	}
	
	Inbox *view = new Inbox(ctx);
	view->push_view();
	return true;
      });

    rdr.cmds.emplace("invite", [&ctx](auto id, auto args) {
	if (args.size() != 1) {
	  log(ctx, "Invalid number of arguments, syntax: invite foo@bar.com");
	  return false;
	}
	
	db::Trans trans(ctx);
	Invite inv(ctx, args[0]);
	load(ctx.db.invites, inv);
	post_msg(inv);	
	log(ctx, fmt("New invite saved to outbox: %0", inv.to));
	db::commit(trans);
	return true;
      });
    
    rdr.cmds.emplace("setup", [&ctx](auto id, auto args) {
	if (!args.empty()) {
	  log(ctx, "Invalid number of arguments, syntax: setup");
	  return false;
	}
	
	Setup *view = new Setup(ctx);
	view->push_view();
	return true;
      });
  }

  static void init_completion(Reader &rdr) {
    auto comp(gtk_entry_completion_new());
    gtk_entry_completion_set_text_column(comp, 0);
    gtk_entry_set_completion(GTK_ENTRY(rdr.entry), comp);
    auto mod(gtk_list_store_new(1, G_TYPE_STRING));
    
    for(auto &cmd: rdr.cmds) {
      GtkTreeIter iter;
      gtk_list_store_append(mod, &iter);
      gtk_list_store_set(mod, &iter, 0, cmd.first.c_str(), -1);
    }

    gtk_entry_completion_set_model(comp, GTK_TREE_MODEL(mod));
    
  }
  
  static bool exec_cmd(Reader &rdr, const str &in) {
    Ctx &ctx(rdr.ctx);
    str in_str(in);
    
    if (in_str == "") {
      if (!rdr.last_cmd) { return false; }
      in_str = *rdr.last_cmd;
    }

    InStream in_words(in_str);
    str id;
    in_words >> id;
    std::vector<str> args;
    str arg;
    while (in_words >> arg) { args.push_back(arg); }
    auto found(rdr.cmds.find(id));

    if (found == rdr.cmds.end()){
      log(ctx, fmt("Unknown command: '%0'", in_str));
      return false;
    } else if (!found->second(id, args)) {
      return false;
    }
    
    rdr.last_cmd = in_str;
    return true;
  }

  static void on_activate(GtkWidget *_, Reader *rdr) {
    if (exec_cmd(*rdr, gtk_entry_get_text(GTK_ENTRY(rdr->entry)))) {
      gtk_entry_set_text(GTK_ENTRY(rdr->entry), "");
    }
  }

  Reader::Reader(Ctx &ctx): ctx(ctx), entry(gtk_entry_new()) {
    init_cmds(*this);
    init_completion(*this);
    add_style(entry, "reader");
    gtk_widget_set_margin_start(entry, 5);
    gtk_widget_set_margin_end(entry, 5);
    g_signal_connect(entry, "activate", G_CALLBACK(on_activate), this);
  }
  
  GtkWidget *Reader::ptr() {
    return entry;
  }
}}
