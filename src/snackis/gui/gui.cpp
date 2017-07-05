#include "snackis/gui/gui.hpp"

namespace snackis {
namespace gui {
  GtkWidget *window, *panels, *main_panel, *left_panel, *right_panel;

  opt<Console> console;
  opt<Reader> reader;
  opt<RootView> root_view;

  str get_str(GtkEntry *w) {
    return gtk_entry_get_text(w);
  }

  str get_str(GtkTextView *w) {
    auto buf(gtk_text_view_get_buffer(w));
    GtkTextIter start, end;
    gtk_text_buffer_get_start_iter(buf, &start);
    gtk_text_buffer_get_end_iter(buf, &end);
    return gtk_text_buffer_get_text(buf, &start, &end, true);
  }

  void set_str(GtkEntry *w, const str &in) {
    gtk_entry_set_text(w, in.c_str());
  }

  void set_str(GtkTextView *w, const str &in) {
    auto buf(gtk_text_view_get_buffer(w));
    gtk_text_buffer_set_text(buf, in.c_str(), in.size());
  }

  opt<GtkTreeIter> get_sel_iter(GtkTreeView *w) {
    GtkTreeSelection *sel(gtk_tree_view_get_selection(w));
    GtkTreeIter iter;
    GtkTreeModel *mod;
    if (!gtk_tree_selection_get_selected(sel, &mod, &iter)) { return nullopt; }
    return iter;
  }

  GtkWidget *new_id_field() {
    GtkWidget *w(gtk_entry_new());
    gtk_entry_set_max_length(GTK_ENTRY(w), ID_LEN);
    gtk_entry_set_width_chars(GTK_ENTRY(w), ID_LEN);
    gtk_entry_set_max_width_chars(GTK_ENTRY(w), ID_LEN);
    return w;
  }

  GtkWidget *new_combo_box(GtkTreeModel *mod) {
    GtkWidget *w(gtk_combo_box_new_with_model(mod));
    gtk_combo_box_set_id_column(GTK_COMBO_BOX(w), 1);
    auto col(gtk_cell_renderer_text_new());
    gtk_cell_layout_pack_start(GTK_CELL_LAYOUT(w), col, true);
    gtk_cell_layout_set_attributes(GTK_CELL_LAYOUT(w),
				   col,
				   "text", 2,
				   nullptr);
    return w;
  }
  
  GtkWidget *new_text_view() {
    GtkWidget *w(gtk_text_view_new());
    gtk_widget_set_hexpand(w, true);
    gtk_widget_set_vexpand(w, true);
    gtk_text_view_set_accepts_tab(GTK_TEXT_VIEW(w), false);
    gtk_text_view_set_wrap_mode(GTK_TEXT_VIEW(w), GTK_WRAP_CHAR);
    auto *scroll(gtk_scrolled_window_new(NULL, NULL));
    gtk_scrolled_window_set_overlay_scrolling(GTK_SCROLLED_WINDOW(scroll), false);
    gtk_scrolled_window_set_policy(GTK_SCROLLED_WINDOW(scroll),
				   GTK_POLICY_NEVER,
				   GTK_POLICY_ALWAYS);
    gtk_container_add(GTK_CONTAINER(scroll), w);
    return w;
  }

  GtkWidget *new_tree_view(GtkTreeModel *mod) {
    GtkWidget *w(gtk_tree_view_new_with_model(mod));
    gtk_widget_set_hexpand(w, true);
    gtk_widget_set_vexpand(w, true);
    auto *scroll(gtk_scrolled_window_new(NULL, NULL));
    gtk_scrolled_window_set_overlay_scrolling(GTK_SCROLLED_WINDOW(scroll), false);
    gtk_scrolled_window_set_policy(GTK_SCROLLED_WINDOW(scroll),
				   GTK_POLICY_NEVER,
				   GTK_POLICY_ALWAYS);
    gtk_container_add(GTK_CONTAINER(scroll), w);
    return w;
  }
  
  GtkTreeViewColumn *add_col(GtkTreeView *w, const str &lbl, int idx, bool ellips) {
    auto rnd(gtk_cell_renderer_text_new());

    if (ellips) {
      g_object_set(G_OBJECT(rnd),
		   "ellipsize", PANGO_ELLIPSIZE_MIDDLE,
		   nullptr);
    }

    auto col(gtk_tree_view_column_new_with_attributes(lbl.c_str(),
						      rnd,
						      "text", idx,
						      nullptr));
    gtk_tree_view_append_column(GTK_TREE_VIEW(w), col);
    return col;
  }

}}
