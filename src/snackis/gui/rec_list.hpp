#ifndef SNACKIS_GUI_REC_LIST_HPP
#define SNACKIS_GUI_REC_LIST_HPP

#include <set>
#include "snackis/gui/search_view.hpp"

namespace snackis {
namespace gui {
  template <typename RecT>
  struct RecList: Widget {
    Ctx &ctx;
    GtkListStore *store;
    GtkWidget *box, *list, *add_btn;
    std::set<UId> &ids;
    
    RecList(Ctx &ctx, const str &lbl, std::set<UId> &ids);
    GtkWidget *ptr() override;
    virtual db::Table<RecT> &table() const=0;
    virtual SearchView<RecT> *search() const=0;
  };

  enum RecListCol {COL_REC_PTR=0, COL_REC_ID, COL_REC_NAME};

  template <typename RecT>
  void on_add_rec(gpointer *_, RecList<RecT> *w) {
    Ctx &ctx(w->ctx);

    auto sv(w->search());
    sv->close_on_activate = true;
    enable_multi_sel(GTK_TREE_VIEW(sv->list));
    
    sv->on_activate = [w, sv, &ctx](auto &rec) {
      RecT obj(ctx, rec);    

      if (w->ids.insert(obj.id).second) {
	GtkTreeIter it;    
	gtk_list_store_append(w->store, &it);
	gtk_list_store_set(w->store, &it,
			   COL_REC_PTR, &rec,
			   COL_REC_ID, id_str(obj).c_str(),
			   COL_REC_NAME, obj.name.c_str(),
			   -1);

	auto sel(gtk_tree_view_get_selection(GTK_TREE_VIEW(w->list)));
	gtk_tree_selection_unselect_all(sel);
	gtk_tree_selection_select_iter(sel, &it);
      }
    };
    
    push_view(sv);
  }

  template <typename RecT>
  void activate(RecList<RecT> *w) {
    each_sel(GTK_TREE_VIEW(w->list), [w](auto it) {
	auto rec(get_rec<RecT>(GTK_TREE_VIEW(w->list), it));
	CHECK(rec, _);
	RecT obj(w->ctx, *rec);
	w->ids.erase(obj.id);
	gtk_list_store_remove(w->store, &it);
      });
  }
  
  template <typename RecT>
  gboolean on_rec_key(gpointer _, GdkEventKey *ev, RecList<RecT> *w) {
    if (ev->keyval != GDK_KEY_Return) { return false; }
    activate(w);
    return true;
  }

  template <typename RecT>
  void on_rec_activate(GtkTreeView *treeview,
		       GtkTreePath *path,
		       GtkTreeViewColumn *col,
		       RecList<RecT> *w) {
    activate(w);
  }
  
  template <typename RecT>
  RecList<RecT>::RecList(Ctx &ctx,
			 const str &lbl,
			 std::set<UId> &ids):
    ctx(ctx),
    store(gtk_list_store_new(3, G_TYPE_POINTER,
			     G_TYPE_STRING,
			     G_TYPE_STRING)),
    box(gtk_box_new(GTK_ORIENTATION_VERTICAL, 5)),
    list(new_tree_view(GTK_TREE_MODEL(store))),
    add_btn(gtk_button_new_with_mnemonic(fmt("_Add %0", lbl).c_str())),
    ids(ids)
  {
    add_col(GTK_TREE_VIEW(list), fmt("%0s", lbl), COL_REC_ID);
    add_col(GTK_TREE_VIEW(list), "", COL_REC_NAME);

    g_signal_connect(list, "key_press_event", G_CALLBACK(on_rec_key<RecT>), this);
    
    enable_multi_sel(GTK_TREE_VIEW(list));
    g_signal_connect(list,
		     "row-activated",
		     G_CALLBACK(on_rec_activate<RecT>),
		     this);    
    gtk_container_add(GTK_CONTAINER(box), gtk_widget_get_parent(list));

    GtkWidget *btn_box(gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 5));
    gtk_container_add(GTK_CONTAINER(box), btn_box);
    
    gtk_widget_set_halign(add_btn, GTK_ALIGN_START);
    g_signal_connect(add_btn, "clicked", G_CALLBACK(on_add_rec<RecT>), this);
    gtk_container_add(GTK_CONTAINER(btn_box), add_btn);

    GtkWidget *l(gtk_label_new(fmt("Press Return or double-click "
				   "to remove selected %0s", lbl).c_str()));
    gtk_widget_set_hexpand(l, true);
    gtk_container_add(GTK_CONTAINER(btn_box), l);
  }

  template <typename RecT>
  GtkWidget *RecList<RecT>::ptr() { return box; }

  template <typename RecT>
  void load(RecList<RecT> &w) {
    gtk_list_store_clear(w.store);
    auto &tbl(w.table());
    
    for (const auto &id: w.ids) {
      db::Rec<RecT> key;
      db::set(key,
	      *dynamic_cast<const db::Col<RecT, UId> *>(tbl.key.cols.front()),
	      id);
      auto rec(db::find(tbl, key));
      
      if (rec) {
	RecT obj(w.ctx, *rec);
	GtkTreeIter iter;
	gtk_list_store_append(w.store, &iter);
	gtk_list_store_set(w.store, &iter,
			   COL_REC_PTR, rec,
			   COL_REC_ID, id_str(obj).c_str(),
			   COL_REC_NAME, obj.name.c_str(),
			   -1);
      }
    }
  }

  template <typename RecT>
  size_t rec_count(RecList<RecT> &w) {
    return gtk_tree_model_iter_n_children(GTK_TREE_MODEL(w.store), nullptr);
  }
}}

#endif
