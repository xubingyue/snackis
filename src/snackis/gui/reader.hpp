#ifndef SNACKIS_GUI_READER_HPP
#define SNACKIS_GUI_READER_HPP

#include <gtk/gtk.h>
#include "snackis/gui/widget.hpp"

namespace snackis {
namespace gui {
  struct Reader: public Widget {
    GtkWidget *entry;
    Reader();
    GtkWidget *ptr();
  };

}}

#endif
