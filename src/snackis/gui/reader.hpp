#ifndef SNACKIS_GUI_READER_HPP
#define SNACKIS_GUI_READER_HPP

#include <gtk/gtk.h>

namespace snackis {
namespace gui {
  struct Reader {
    GtkWidget *entry;
    Reader();
  };

  GtkWidget *widget(Reader &reader);
}}

#endif
