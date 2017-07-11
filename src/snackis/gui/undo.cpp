#include "snackis/ctx.hpp"
#include "snackis/gui/feed_view.hpp"
#include "snackis/gui/gui.hpp"
#include "snackis/gui/undo.hpp"
#include "snackis/gui/reader.hpp"
#include "snackis/gui/feed_view.hpp"
#include "snackis/gui/peer_view.hpp"
#include "snackis/gui/post_view.hpp"
#include "snackis/gui/project_view.hpp"
#include "snackis/gui/task_view.hpp"

namespace snackis {
namespace gui {
  enum Cols { COL_TIME=0, COL_LABEL };
  
  static void on_cancel(gpointer *_, Undo *v) {
    pop_view(v);
  }

  static void on_undo(gpointer *_, Undo *v) {
    Ctx &ctx(v->ctx);
    CHECK(!ctx.undo_stack.empty(), _);
    db::Trans trans(ctx);
    TRY(try_undo);
    auto &cs(ctx.undo_stack.back());
    db::undo(cs);
    
    if (try_undo.errors.empty()) {
      db::commit(trans, nullopt);

      GtkTreeIter it;
      gtk_tree_model_get_iter_first(GTK_TREE_MODEL(v->store), &it);
      gtk_list_store_remove(v->store, &it);
      gtk_widget_set_sensitive(v->revert_btn, !ctx.undo_stack.empty());
      log(ctx, fmt("Reverted change:\n%0", cs.label)); 
    }

    ctx.undo_stack.pop_back();
  }
  
  Undo::Undo(Ctx &ctx):
    View(ctx, "Undo"),
    store(gtk_list_store_new(2,
			     G_TYPE_STRING, G_TYPE_STRING)),
    lst(new_tree_view(GTK_TREE_MODEL(store))),
    revert_btn(gtk_button_new_with_mnemonic("_Revert Last Change")),
    cancel_btn(gtk_button_new_with_mnemonic("_Cancel"))
  {
    add_col(GTK_TREE_VIEW(lst), "Time", COL_TIME);
    add_col(GTK_TREE_VIEW(lst), "Change", COL_LABEL, true);
    gtk_container_add(GTK_CONTAINER(panel), gtk_widget_get_parent(lst));
    GtkWidget *lst_btns(gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 5));
    gtk_container_add(GTK_CONTAINER(panel), lst_btns);
    g_signal_connect(revert_btn, "clicked", G_CALLBACK(on_undo), this);
    gtk_container_add(GTK_CONTAINER(lst_btns), revert_btn);
    
    gtk_widget_set_margin_top(cancel_btn, 10);
    gtk_widget_set_halign(cancel_btn, GTK_ALIGN_END);
    g_signal_connect(cancel_btn, "clicked", G_CALLBACK(on_cancel), this);
    gtk_box_pack_start(GTK_BOX(panel), cancel_btn, false, false, 0);
    focused = lst;
  }

  void Undo::load() {
    View::load();
    gtk_list_store_clear(store);

    for(auto cs=ctx.undo_stack.rbegin(); cs != ctx.undo_stack.rend(); cs++) {
      GtkTreeIter it;
      gtk_list_store_append(store, &it);
      gtk_list_store_set(store, &it,
			 COL_TIME, fmt(cs->committed_at, "%a %d, %H:%M").c_str(),
			 COL_LABEL, cs->label.c_str(),
			 -1);
    }
  }
}}
