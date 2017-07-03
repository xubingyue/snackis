#ifndef SNACKIS_GUI_SEARCH_VIEW_HPP
#define SNACKIS_GUI_SEARCH_VIEW_HPP

#include <mutex>
#include <set>
#include <vector>

#include "snackis/core/error.hpp"
#include "snackis/core/func.hpp"
#include "snackis/gui/gui.hpp"
#include "snackis/gui/view.hpp"

namespace snackis {
namespace gui {
  template <typename RecT>
  struct SearchView: View {
    using OnActivate = func<void (const db::Rec<RecT> &)>;
    GtkListStore *store;
    GtkWidget *fields, *find_btn, *list, *cancel_btn;
    OnActivate on_activate;
    
    SearchView(Ctx &ctx, const str &lbl, GtkListStore *store, OnActivate act);
    virtual void init() override;
    virtual void find()=0;
  };

  template <typename RecT>
  void on_search_find(gpointer *_, SearchView<RecT> *v) {
    gtk_list_store_clear(v->store);
    v->find();
    gtk_widget_show(v->list);
  }
  
  template <typename RecT>
  void on_search_activate(GtkTreeView *treeview,
		      GtkTreePath *path,
		      GtkTreeViewColumn *col,
		      SearchView<RecT> *v) {
    auto rec(get_sel_rec<RecT>(GTK_TREE_VIEW(v->list)));
    CHECK(rec != nullptr, _);
    v->on_activate(*rec);
  }
  
  template <typename RecT>
  void on_search_cancel(gpointer *_, SearchView<RecT> *v) {
    pop_view(*v);
  }
  
  template <typename RecT>
  SearchView<RecT>::SearchView(Ctx &ctx,
			       const str &lbl,
			       GtkListStore *store,
			       OnActivate act):
    View(ctx, fmt("%0 Search", lbl)),
    store(store),
    fields(gtk_box_new(GTK_ORIENTATION_VERTICAL, 5)),
    find_btn(gtk_button_new_with_mnemonic(fmt("_Find %0s", lbl).c_str())),
    list(gtk_tree_view_new_with_model(GTK_TREE_MODEL(store))),
    cancel_btn(gtk_button_new_with_mnemonic("_Cancel")),
    on_activate(act)
  { }

  template <typename RecT>
  void SearchView<RecT>::init() {
    GtkWidget *lbl;
    gtk_box_pack_start(GTK_BOX(panel), fields, false, false, 0);

    gtk_widget_set_halign(find_btn, GTK_ALIGN_END);
    gtk_widget_set_margin_top(find_btn, 10);
    g_signal_connect(find_btn, "clicked", G_CALLBACK(on_search_find<RecT>), this);
    gtk_container_add(GTK_CONTAINER(panel), find_btn);

    g_signal_connect(list,
		     "row-activated",
		     G_CALLBACK(on_search_activate<RecT>),
		     this);
    gtk_box_pack_start(GTK_BOX(panel), list, true, true, 0);
    lbl = gtk_label_new("Press Return or double-click to select");
    gtk_container_add(GTK_CONTAINER(panel), lbl);

    GtkWidget *btns = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 5);
    gtk_widget_set_halign(btns, GTK_ALIGN_END);
    gtk_widget_set_valign(btns, GTK_ALIGN_END);
    gtk_widget_set_margin_top(btns, 10);
    gtk_container_add(GTK_CONTAINER(panel), btns);
        
    g_signal_connect(cancel_btn, "clicked", G_CALLBACK(on_search_cancel<RecT>), this);
    gtk_container_add(GTK_CONTAINER(btns), cancel_btn);
    focused = fields;
  }
}}

#endif