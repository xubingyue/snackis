#include "snackis/ctx.hpp"
#include "snackis/gui/feed_view.hpp"
#include "snackis/gui/gui.hpp"
#include "snackis/gui/todo.hpp"
#include "snackis/gui/reader.hpp"
#include "snackis/gui/feed_view.hpp"
#include "snackis/gui/peer_view.hpp"
#include "snackis/gui/post_view.hpp"
#include "snackis/gui/project_view.hpp"
#include "snackis/gui/task_view.hpp"

namespace snackis {
namespace gui {
  enum Cols { COL_PTR=0, COL_INFO, COL_PRIO, COL_DONE };
  
  static void on_cancel(gpointer *_, Todo *v) {
    pop_view(v);
  }

  static void on_activate(GtkTreeView *tree,
			  GtkTreePath *path,
			  GtkTreeViewColumn *col,
			  Todo *v) {
    auto rec(get_sel_rec<Task>(tree));
    CHECK(rec, _);
    push_view(new TaskView(Task(v->ctx, *rec)));
  }
  
  Todo::Todo(Ctx &ctx):
    View(ctx, "Todo"),
    store(gtk_list_store_new(4,
			     G_TYPE_POINTER,
			     G_TYPE_STRING, G_TYPE_STRING, G_TYPE_STRING)),
    lst(new_tree_view(GTK_TREE_MODEL(store))),
    cancel_btn(gtk_button_new_with_mnemonic("_Cancel"))
  {
    add_col(GTK_TREE_VIEW(lst), "Info", COL_INFO, true);
    add_col(GTK_TREE_VIEW(lst), "Prio", COL_PRIO);
    
    auto done_col(add_col(GTK_TREE_VIEW(lst), "Done", COL_DONE));
    g_object_set(done_col.second,
		 "foreground", "DarkGreen",
		 "foreground-set", true,
		 nullptr);
    
    g_signal_connect(lst, "row-activated", G_CALLBACK(on_activate), this);
    gtk_container_add(GTK_CONTAINER(panel), gtk_widget_get_parent(lst));

    gtk_container_add(GTK_CONTAINER(panel),
		      new_hint("Press Return or double-click to view Task,\n"
			       "or press F5 to refresh."));
     
    gtk_widget_set_margin_top(cancel_btn, 10);
    gtk_widget_set_halign(cancel_btn, GTK_ALIGN_END);
    g_signal_connect(cancel_btn, "clicked", G_CALLBACK(on_cancel), this);
    gtk_box_pack_start(GTK_BOX(panel), cancel_btn, false, false, 0);
    focused = lst;
  }

  void Todo::load() {
    View::load();
    gtk_list_store_clear(store);
    refresh(ctx);
    size_t cnt(0);
    
    for(const auto &key: ctx.db.tasks_sort.recs) {
      auto &rec(db::get(ctx.db.tasks, key.second));
      Task tsk(ctx, rec);
      
      if (tsk.tags.find("todo") == tsk.tags.end()) { continue; }

      if (tsk.done &&
	  tsk.done_at < now() - std::chrono::hours(TODO_DONE_DAYS*24)) {
	continue;
      }
	
      GtkTreeIter iter;
      gtk_list_store_append(store, &iter);
      Project prj(get_project_id(ctx, tsk.project_id));
      
      gtk_list_store_set(store, &iter,
			 COL_PTR, &rec,
			 COL_INFO, fmt("%0\n%1", prj.name, tsk.name).c_str(),
			 COL_PRIO, to_str(tsk.prio).c_str(),
			 COL_DONE, tsk.done ? "Done!" : "",
			 -1);
      cnt++;
    }

    if (cnt) {
      sel_first(GTK_TREE_VIEW(lst));
      gtk_widget_grab_focus(lst);
    }
  }
}}
