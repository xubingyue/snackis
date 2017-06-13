#include "snackis/gui/gui.hpp"

namespace snackis {
namespace gui {
  GtkWidget *window, *panels, *status;
  
  void quit() {
    gtk_widget_destroy(GTK_WIDGET(window));
  }

  void push_status(const str &msg) {
    gtk_statusbar_push(GTK_STATUSBAR(status), 0, msg.c_str());
  }
}}
