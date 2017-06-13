#include "snackis/ctx.hpp"
#include "snackis/core/fmt.hpp"
#include "snackis/gui/gui.hpp"
#include "snackis/gui/reader.hpp"
#include "snackis/gui/widget.hpp"

namespace snackis {
namespace gui {
  static void on_activate(GtkWidget *_, Reader *rdr) {
    str in(gtk_entry_get_text(GTK_ENTRY(rdr->entry)));
    
    if (in == "") {
      if (!rdr->last_cmd) { return; }
      in = *rdr->last_cmd;
    }
    
    if (in == "quit") { quit(); }
    else if (in == "setup") {
      Setup *setup = new Setup(rdr->ctx);
      setup->push_view();
    } else {
      log(rdr->ctx, fmt("Unknown command: '%0'", in));
      return;
    }
    
    rdr->last_cmd = in;
    gtk_entry_set_text(GTK_ENTRY(rdr->entry), "");
  }
  
  Reader::Reader(Ctx &ctx): ctx(ctx), entry(gtk_entry_new()) {
    add_style(entry, "reader");
    g_signal_connect(entry, "activate", G_CALLBACK(on_activate), this);
  }
  
  GtkWidget *Reader::ptr() {
    return entry;
  }
}}
