#include "snackis/gui/reader.hpp"
#include "snackis/gui/widget.hpp"

namespace snackis {
namespace gui {
  Reader::Reader(): entry(gtk_entry_new()) {
    add_style(entry, "reader");
  }

  GtkWidget *widget(Reader &reader) {
    return reader.entry;
  }
}}
