#ifndef SNACKIS_GUI_HPP
#define SNACKIS_GUI_HPP

#include <gtk/gtk.h>
#include "snackis/core/opt.hpp"
#include "snackis/core/str.hpp"
#include "snackis/db/rec.hpp"
#include "snackis/gui/console.hpp"
#include "snackis/gui/login.hpp"
#include "snackis/gui/reader.hpp"
#include "snackis/gui/root_view.hpp"
#include "snackis/gui/setup.hpp"

namespace snackis {
namespace gui {
  const int ID_LEN(8);
  
  extern GtkWidget *window, *panels, *main_panel, *left_panel;
  
  extern opt<Console> console;
  extern opt<Reader> reader;
  extern opt<RootView> root_view;

  str get_str(GtkEntry *w);
  str get_str(GtkTextView *w);
  void set_str(GtkEntry *w, const str &in);
  void set_str(GtkTextView *w, const str &in);

  template <typename RecT>
  const db::Rec<RecT> *get_sel_rec(GtkComboBox *w) {
    GtkTreeIter iter;
    if (!gtk_combo_box_get_active_iter(w, &iter)) { return nullptr; }
    const db::Rec<RecT> *rec(nullptr);
    GtkTreeModel *mod(gtk_combo_box_get_model(w));
    gtk_tree_model_get(mod, &iter, 0, &rec, -1); 
    return rec;
  }

  opt<GtkTreeIter> get_sel_iter(GtkTreeView *w);

  template <typename RecT>
  const db::Rec<RecT> *get_sel_rec(GtkTreeView *w, GtkTreeIter &it) {
    const db::Rec<RecT> *rec(nullptr);
    gtk_tree_model_get(gtk_tree_view_get_model(w), &it, 0, &rec, -1);
    return rec;
  }

  template <typename RecT>
  const db::Rec<RecT> *get_sel_rec(GtkTreeView *w) {
    auto iter(get_sel_iter(w));
    if (!iter) { return nullptr; }
    return get_sel_rec<RecT>(w, *iter);
  }

  GtkWidget *new_id_field();
  GtkWidget *new_combo_box(GtkTreeModel *mod);
  GtkWidget *new_text_view();
  GtkWidget *new_tree_view(GtkTreeModel *mod);

  GtkTreeViewColumn *add_col(GtkTreeView *w, const str &lbl, int idx,
			     bool exp=false);
}}

#endif
