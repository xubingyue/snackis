#include <cassert>
#include "snackis/gui/gui.hpp"
#include "snackis/gui/console.hpp"
#include "snackis/gui/widget.hpp"

namespace snackis {
namespace gui {
  Console::Console():
    text_view(gtk_text_view_new()), scroll_view(gtk_scrolled_window_new(NULL, NULL)) {
    add_style(text_view, "console");
    gtk_text_view_set_editable(GTK_TEXT_VIEW(text_view), false);
    gtk_scrolled_window_set_overlay_scrolling(GTK_SCROLLED_WINDOW(scroll_view),
					      false);
    gtk_scrolled_window_set_policy(GTK_SCROLLED_WINDOW(scroll_view),
				   GTK_POLICY_NEVER,
				   GTK_POLICY_ALWAYS);
    gtk_container_add(GTK_CONTAINER(scroll_view), text_view);
  }

  GtkWidget *widget(Console &cns) {
    return cns.scroll_view;
  }
}}
