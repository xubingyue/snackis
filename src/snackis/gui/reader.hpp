#ifndef SNACKIS_GUI_READER_HPP
#define SNACKIS_GUI_READER_HPP

#include <gtk/gtk.h>
#include "snackis/gui/widget.hpp"

namespace snackis {
namespace gui {
  struct Reader: public Widget {
    Ctx &ctx;
    GtkWidget *entry;
    opt<str> last_cmd;
    
    Reader(Ctx &ctx);
    GtkWidget *ptr();
  };

}}

#endif
