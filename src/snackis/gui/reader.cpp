#include "snackis/ctx.hpp"
#include "snackis/invite.hpp"
#include "snackis/core/fmt.hpp"
#include "snackis/gui/gui.hpp"
#include "snackis/gui/reader.hpp"
#include "snackis/gui/widget.hpp"

namespace snackis {
namespace gui {
  static bool exec_cmd(Reader &rdr, const str &in) {
    Ctx &ctx(rdr.ctx);
    str in_str(in);
    
    if (in_str == "") {
      if (!rdr.last_cmd) { return false; }
      in_str = *rdr.last_cmd;
    }

    InStream in_words(in_str);
    str cmd;
    in_words >> cmd;
    std::vector<str> args;
    str arg;
    while (in_words >> arg) { args.push_back(arg); }
    
    if (cmd == "invite") {
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
    } else if (cmd == "quit") {
      if (!args.empty()) {
	log(ctx, "Invalid number of arguments, syntax: quit");
	return false;
      }
      
      quit();
    }
    else if (cmd == "setup") {
      if (!args.empty()) {
	log(ctx, "Invalid number of arguments, syntax: setup");
	return false;
      }

      Setup *setup = new Setup(ctx);
      setup->push_view();
    } else {
      log(ctx, fmt("Unknown command: '%0'", in_str));
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
    add_style(entry, "reader");
    gtk_widget_set_margin_start(entry, 5);
    gtk_widget_set_margin_end(entry, 5);
    g_signal_connect(entry, "activate", G_CALLBACK(on_activate), this);
  }
  
  GtkWidget *Reader::ptr() {
    return entry;
  }
}}
