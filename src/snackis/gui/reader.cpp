#include "snackis/gui/gui.hpp"
#include "snackis/gui/reader.hpp"
#include "snackis/gui/widget.hpp"

namespace snackis {
namespace gui {
  static void on_activate(GtkWidget *entry) {
    const str in(gtk_entry_get_text(GTK_ENTRY(entry)));

    if (in == "quit") { quit(); }
  }
  
  Reader::Reader(): entry(gtk_entry_new()) {
    add_style(entry, "reader");
    g_signal_connect(entry, "activate", G_CALLBACK(on_activate), NULL);
  }

  GtkWidget *widget(Reader &reader) {
    return reader.entry;
  }
}}
