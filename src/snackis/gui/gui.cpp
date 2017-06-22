#include "snackis/gui/gui.hpp"

namespace snackis {
namespace gui {
  GtkWidget *window, *panels, *main_panel, *left_panel;

  opt<Login> login;
  opt<Console> console;
  opt<Reader> reader;
  opt<Setup> setup;

  str get_str(GtkTextView *tv) {
    auto buf(gtk_text_view_get_buffer(tv));
    GtkTextIter start, end;
    gtk_text_buffer_get_start_iter(buf, &start);
    gtk_text_buffer_get_end_iter(buf, &end);
    return gtk_text_buffer_get_text(buf, &start, &end, true);
  }

  void set_str(GtkTextView *tv, const str &in) {
    auto buf(gtk_text_view_get_buffer(tv));
    gtk_text_buffer_set_text(buf, in.c_str(), in.size());
  }
}}
