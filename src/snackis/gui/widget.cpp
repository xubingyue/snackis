#include "snackis/gui/widget.hpp"

namespace snackis {
namespace gui {
  void add_style(GtkWidget *wgt, const str &style) {
    gtk_style_context_add_class(gtk_widget_get_style_context(wgt), style.c_str());
  }

  GtkWidget *ptr(Widget &wgt) {
    return wgt.ptr();
  }
  
  void focus(Widget &wgt) {
    gtk_widget_grab_focus(ptr(wgt));
  }
}}
