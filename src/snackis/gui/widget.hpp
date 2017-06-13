#ifndef SNACKIS_GUI_WIDGET_HPP
#define SNACKIS_GUI_WIDGET_HPP

#include <gtk/gtk.h>
#include "snackis/core/str.hpp"

namespace snackis {
namespace gui {
  struct Widget {
    virtual GtkWidget *ptr() = 0;
    virtual void focus();
  };

  void add_style(GtkWidget *wgt, const str &style);
  GtkWidget *ptr(Widget &wgt);
  void focus(Widget &wgt);
}}


#endif
