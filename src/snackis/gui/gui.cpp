#include "snackis/gui/gui.hpp"

namespace snackis {
namespace gui {
  GtkWidget *window, *panels, *main_panel, *left_panel, *right_panel;

  opt<Console> console;
  opt<Reader> reader;
  
  std::unique_ptr<RootView> root_view;
  std::unique_ptr<Setup> setup;
  std::unique_ptr<Inbox> inbox;
  std::unique_ptr<Undo> undo;
  std::unique_ptr<Todo> todo;

  str get_str(GtkEntry *w) {
    return trim(gtk_entry_get_text(w));
  }

  str get_str(GtkTextView *w) {
    auto buf(gtk_text_view_get_buffer(w));
    GtkTextIter start, end;
    gtk_text_buffer_get_start_iter(buf, &start);
    gtk_text_buffer_get_end_iter(buf, &end);
    return trim(gtk_text_buffer_get_text(buf, &start, &end, true));
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

  void each_sel(GtkTreeView *w, func<void (GtkTreeIter &)> fn) {
    std::vector<GtkTreeRowReference *> refs;
    GtkTreeModel *mod;
    auto sel(gtk_tree_selection_get_selected_rows(gtk_tree_view_get_selection(w),
						  &mod));
    
    while (sel) {
      auto pth(static_cast<GtkTreePath *>(sel->data));
      refs.push_back(gtk_tree_row_reference_new(mod, pth));
      sel = sel->next;
    }
    
    g_list_free_full(sel, (GDestroyNotify)gtk_tree_path_free);

    for (auto ref: refs) {
      auto pth(gtk_tree_row_reference_get_path(ref));
      GtkTreeIter it;
      gtk_tree_model_get_iter(mod, &it, pth);
      fn(it);
      gtk_tree_row_reference_free(ref);
    }
  }

  void enable_multi_sel(GtkTreeView *w) {
    gtk_tree_selection_set_mode(gtk_tree_view_get_selection(w),
				GTK_SELECTION_MULTIPLE);
  }

  bool sel_first(GtkTreeView *w) {
      auto sel(gtk_tree_view_get_selection(w));
      gtk_tree_selection_unselect_all(sel);
      GtkTreeIter it;

      auto mod(gtk_tree_view_get_model(w));
      if (!gtk_tree_model_get_iter_first(mod, &it)) { return false; }
      gtk_tree_selection_select_iter(sel, &it);
      return true;
  }
  
  GtkWidget *new_grid() {
    GtkWidget *w(gtk_grid_new());
    gtk_grid_set_row_spacing(GTK_GRID(w), 5);
    gtk_grid_set_column_spacing(GTK_GRID(w), 5);
    return w;
  }
  
  GtkWidget *new_label(const str &txt) {
    GtkWidget *w(gtk_label_new(txt.c_str()));
    gtk_widget_set_halign(w, GTK_ALIGN_START);
    return w;
  }

  void set_width(GtkEntry *e, int w) {
    gtk_entry_set_max_length(GTK_ENTRY(e), w);
    gtk_entry_set_width_chars(GTK_ENTRY(e), w+1);
    gtk_entry_set_max_width_chars(GTK_ENTRY(e), w+1);
  }
  
  GtkWidget *new_id_field() {
    GtkWidget *w(gtk_entry_new());
    set_width(GTK_ENTRY(w), ID_LEN);
    gtk_widget_set_halign(w, GTK_ALIGN_START);
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
    gtk_tree_view_set_headers_visible(GTK_TREE_VIEW(w), false);
    auto *scroll(gtk_scrolled_window_new(NULL, NULL));
    gtk_scrolled_window_set_overlay_scrolling(GTK_SCROLLED_WINDOW(scroll), false);
    gtk_scrolled_window_set_policy(GTK_SCROLLED_WINDOW(scroll),
				   GTK_POLICY_NEVER,
				   GTK_POLICY_ALWAYS);
    gtk_container_add(GTK_CONTAINER(scroll), w);
    return w;
  }
  
  std::pair<GtkTreeViewColumn *, GtkCellRenderer *>
  add_col(GtkTreeView *w, const str &lbl, int idx, bool exp) {
    auto rnd(gtk_cell_renderer_text_new());

    if (exp) {
      g_object_set(G_OBJECT(rnd),
		   "ellipsize", PANGO_ELLIPSIZE_MIDDLE,
		   nullptr);
    }

    auto col(gtk_tree_view_column_new_with_attributes(lbl.c_str(),
						      rnd,
						      "text", idx,
						      nullptr));
    if (exp) { gtk_tree_view_column_set_expand(col, true); }	    
    gtk_tree_view_append_column(GTK_TREE_VIEW(w), col);
    return std::make_pair(col, rnd);
  }

  std::pair<GtkTreeViewColumn *, GtkCellRenderer *>
  add_check_col(GtkTreeView *w, const str &lbl, int idx) {
    auto rnd(gtk_cell_renderer_toggle_new());

    auto col(gtk_tree_view_column_new_with_attributes(lbl.c_str(),
						      rnd,
						      "active", idx,
						      nullptr));
    gtk_tree_view_append_column(GTK_TREE_VIEW(w), col);
    return std::make_pair(col, rnd);
  }
}}
