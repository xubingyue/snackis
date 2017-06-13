#include "snackis/gui/gui.hpp"

namespace snackis {
namespace gui {
  GtkWidget *window, *panels, *left_panel, *status;

  opt<Login> login;
  opt<Console> console;
  opt<Reader> reader;

  void quit() {
    gtk_widget_destroy(window);
  }

  void push_status(const str &msg) {
    gtk_statusbar_push(GTK_STATUSBAR(status), 0, msg.c_str());
  }
}}
