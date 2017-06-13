#ifndef SNACKIS_GUI_CONSOLE_HPP
#define SNACKIS_GUI_CONSOLE_HPP

#include <gtk/gtk.h>
#include "snackis/core/fmt.hpp"
#include "snackis/core/opt.hpp"
#include "snackis/core/str.hpp"
#include "snackis/core/time.hpp"

namespace snackis {
namespace gui {
  struct Console {
    GtkWidget *text_view, *scroll_view;
    Console();
  };

  GtkWidget *widget(Console &console);

  template <typename...Args>
  void log(Console &cns, const str &spec, Args...args) {
    const str msg(fmt("%0 %1\n", 
		      fmt(now(), "%a %H:%M:%S"), 
		      fmt(spec, std::forward<Args>(args)...)));

    auto buf(gtk_text_view_get_buffer(GTK_TEXT_VIEW(cns.text_view)));
    GtkTextIter end;
    gtk_text_buffer_get_end_iter(buf, &end);
    gtk_text_buffer_place_cursor(buf, &end);
    gtk_text_buffer_insert_at_cursor(buf, msg.c_str(), msg.size());

  }
}}

#endif
