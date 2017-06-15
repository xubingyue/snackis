#include "snackis/gui/gui.hpp"

namespace snackis {
namespace gui {
  GtkWidget *window, *panels, *main_panel, *left_panel;

  opt<Login> login;
  opt<Console> console;
  opt<Reader> reader;
  opt<Setup> setup;

  void quit() {
    gtk_widget_destroy(window);
  }
}}
