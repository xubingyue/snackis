#include <cassert>
#include "snackis/core/fmt.hpp"
#include "snackis/core/time.hpp"
#include "snackis/gui/gui.hpp"
#include "snackis/gui/console.hpp"
#include "snackis/gui/widget.hpp"

namespace snackis {
namespace gui {
  static void refresh(Console &cns) {
    Console::LogLock lock(cns.log_mutex);

    for (const auto &msg: cns.out) {
      auto buf(gtk_text_view_get_buffer(GTK_TEXT_VIEW(cns.view)));
      GtkTextIter start;
      gtk_text_buffer_get_start_iter(buf, &start);
      gtk_text_buffer_place_cursor(buf, &start);
      gtk_text_buffer_insert_at_cursor(buf, msg.c_str(), msg.size());
    }

    cns.out.clear();
  }

  static gboolean on_idle(gpointer cns) {
    refresh(*static_cast<Console *>(cns));
    return false;
  }
  
  void log(Console &cns, const str msg) {
    const str m(fmt("%0 %1\n", fmt(now(), "%a %H:%M:%S"), msg));
    Console::LogLock lock(cns.log_mutex);
    cns.out.push_back(m);
    g_idle_add(on_idle, &cns);
  }

  void clear(Console &cns) {
    auto buf(gtk_text_view_get_buffer(GTK_TEXT_VIEW(cns.view)));
    GtkTextIter start, end;
    gtk_text_buffer_get_start_iter(buf, &start);
    gtk_text_buffer_get_end_iter(buf, &end);
    gtk_text_buffer_delete(buf, &start, &end);
    log(cns, "Cleared console");
  }

  Console::Console(): view(new_text_view()) {
    add_style(view, "console");
    gtk_text_view_set_editable(GTK_TEXT_VIEW(view), false);
    gtk_text_view_set_monospace(GTK_TEXT_VIEW(view), true);
  }

  GtkWidget *Console::ptr() {
    return gtk_widget_get_parent(view);
  }
}}
