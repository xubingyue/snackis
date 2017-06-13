#ifndef SNACKIS_GUI_HPP
#define SNACKIS_GUI_HPP

#include <gtk/gtk.h>
#include "snackis/core/str.hpp"

namespace snackis {
namespace gui {
  extern GtkWidget *window, *panels, *status;
    
  void quit();
  void push_status(const str &msg);
}}

#endif
