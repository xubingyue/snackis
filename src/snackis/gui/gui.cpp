#include "snackis/gui/gui.hpp"

namespace snackis {
namespace gui {
  GtkWidget *window;
  
  void quit() {
    gtk_widget_destroy(GTK_WIDGET(window));
  }
}}
