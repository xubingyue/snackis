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
    sv->on_activate = [w, sv, &ctx](auto &rec) {
      RecT obj(ctx, rec);    

      if (w->ids.insert(obj.id).second) {
	GtkTreeIter iter;    
	gtk_list_store_append(w->store, &iter);
	gtk_list_store_set(w->store, &iter,
			   COL_REC_PTR, &rec,
			   COL_REC_ID, id_str(obj).c_str(),
			   COL_REC_NAME, obj.name.c_str(),
			   -1);
	auto sel(gtk_tree_view_get_selection(GTK_TREE_VIEW(w->list)));
	gtk_tree_selection_select_iter(sel, &iter);
      }
      
      pop_view(*sv);
      gtk_widget_grab_focus(w->add_btn);
    };
    
    push_view(*sv);
  }

  template <typename RecT>
  void on_remove_rec(GtkTreeView *treeview,
		     GtkTreePath *path,
		     GtkTreeViewColumn *col,
		     RecList<RecT> *w) {
    Ctx &ctx(w->ctx);
    auto iter(get_sel_iter(GTK_TREE_VIEW(w->list)));

    if (iter) {
      auto it(*iter);
      auto rec(get_sel_rec<RecT>(GTK_TREE_VIEW(w->list), it));
      CHECK(rec ? true : false, _);
      RecT obj(ctx, *rec);
      w->ids.erase(obj.id);
      gtk_list_store_remove(w->store, &it);
    }
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
    add_btn(gtk_button_new_with_label(fmt("Add %0", lbl).c_str())),
    ids(ids)
  {
    auto list_box(gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 5));
    gtk_container_add(GTK_CONTAINER(box), list_box);
    add_col(GTK_TREE_VIEW(list), fmt("%0s", lbl), COL_REC_ID);
    add_col(GTK_TREE_VIEW(list), "", COL_REC_NAME);
    g_signal_connect(list, "row-activated", G_CALLBACK(on_remove_rec<RecT>), this);
    gtk_container_add(GTK_CONTAINER(list_box), gtk_widget_get_parent(list));
    gtk_widget_set_valign(add_btn, GTK_ALIGN_END);
    g_signal_connect(add_btn, "clicked", G_CALLBACK(on_add_rec<RecT>), this);
    gtk_container_add(GTK_CONTAINER(list_box), add_btn);
    gtk_container_add(GTK_CONTAINER(box),
		      gtk_label_new(fmt("Press Return or double-click to remove %0",
					lbl).c_str()));
  }

  template <typename RecT>
  GtkWidget *RecList<RecT>::ptr() { return box; }

  template <typename RecT>
  void load(RecList<RecT> &w) {
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
}}

#endif
