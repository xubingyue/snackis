#ifndef SNACKIS_GUI_SEARCH_VIEW_HPP
#define SNACKIS_GUI_SEARCH_VIEW_HPP

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
    bool close_on_activate;
    
    SearchView(Ctx &ctx, const str &type, GtkListStore *store, OnActivate act);
    virtual void find()=0;
  };

  template <typename RecT>
  size_t find(SearchView<RecT> &v) {
    TRY(try_find);
    gtk_list_store_clear(v.store);
    v.find();

    auto cnt(gtk_tree_model_iter_n_children(GTK_TREE_MODEL(v.store), nullptr));

    if (cnt) {
      auto sel(gtk_tree_view_get_selection(GTK_TREE_VIEW(v.list)));
      gtk_tree_selection_unselect_all(sel);
      GtkTreeIter it;
      CHECK(gtk_tree_model_get_iter_first(GTK_TREE_MODEL(v.store), &it), _);
      gtk_tree_selection_select_iter(sel, &it);
    }
    
    return cnt;
  }
  
  template <typename RecT>
  void on_search_find(gpointer *_, SearchView<RecT> *v) {
    find(*v);
  }
  
  template <typename RecT>
  gboolean on_search_key(gpointer _, GdkEventKey *ev, SearchView<RecT> *v) {
    if (ev->keyval != GDK_KEY_Return) { return false; }    
    TRY(try_activate);

    each_sel(GTK_TREE_VIEW(v->list), [v](auto it) {
	auto rec(get_rec<RecT>(GTK_TREE_VIEW(v->list), it));
	CHECK(rec, _);
	v->on_activate(*rec);
      });
    
    if (v->close_on_activate) {
      pop_view(v);
      delete v;
    }
    
    return true;
  }
  
  template <typename RecT>
  void on_search_cancel(gpointer *_, SearchView<RecT> *v) {
    pop_view(v);
    delete v;
  }
  
  template <typename RecT>
  SearchView<RecT>::SearchView(Ctx &ctx,
			       const str &type,
			       GtkListStore *store,
			       OnActivate act):
    View(ctx, fmt("%0 Search", type)),
    store(store),
    fields(gtk_box_new(GTK_ORIENTATION_VERTICAL, 5)),
    find_btn(gtk_button_new_with_mnemonic(fmt("_Find %0s", type).c_str())),
    list(new_tree_view(GTK_TREE_MODEL(store))),
    cancel_btn(gtk_button_new_with_mnemonic("_Cancel")),
    on_activate(act),
    close_on_activate(false)
  {
    GtkWidget *lbl;
    gtk_box_pack_start(GTK_BOX(panel), fields, false, false, 0);
    
    gtk_widget_set_halign(find_btn, GTK_ALIGN_END);
    gtk_widget_set_margin_top(find_btn, 10);
    g_signal_connect(find_btn, "clicked", G_CALLBACK(on_search_find<RecT>), this);
    gtk_container_add(GTK_CONTAINER(panel), find_btn);

    gtk_widget_set_margin_top(list, 5);
    g_signal_connect(list,
		     "key_press_event",
		     G_CALLBACK(on_search_key<RecT>),
		     this);
    gtk_container_add(GTK_CONTAINER(panel), gtk_widget_get_parent(list));
    lbl = gtk_label_new(fmt("Press Return to select %0",
			    type).c_str());
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
  
  template <typename RecT>
  const db::Rec<RecT> *first_rec(const SearchView<RecT> &v) {
    GtkTreeIter it;

    if (!gtk_tree_model_get_iter_first(GTK_TREE_MODEL(v.store), &it)) {
      return nullptr;
    }
    
    return gui::get_rec<RecT>(GTK_TREE_VIEW(v.list), it);
  }
}}

#endif
