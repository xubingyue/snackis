#ifndef SNACKIS_GUI_HPP
#define SNACKIS_GUI_HPP

#include <gtk/gtk.h>
#include "snackis/core/opt.hpp"
#include "snackis/core/str.hpp"
#include "snackis/gui/console.hpp"
#include "snackis/gui/login.hpp"
#include "snackis/gui/reader.hpp"

namespace snackis {
namespace gui {
  extern GtkWidget *window, *panels, *left_panel, *status;
  
  extern opt<Login> login;
  extern opt<Console> console;
  extern opt<Reader> reader;
  
  void quit();
  void push_status(const str &msg);
}}

#endif
