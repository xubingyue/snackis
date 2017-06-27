#include <cassert>
#include "snackis/ctx.hpp"
#include "snackis/gui/gui.hpp"
#include "snackis/gui/task_search.hpp"
#include "snackis/gui/task_view.hpp"

namespace snackis {
namespace gui {
  enum PeerCol {COL_PEER_PTR=0, COL_PEER_ID, COL_PEER_NAME};
  enum TaskCol {COL_TASK_PTR=0, COL_TASK_CREATED, COL_TASK_OWNER,
		COL_TASK_PROJECT, COL_TASK_NAME, COL_TASK_INFO};

  static void on_find(gpointer *_, TaskSearch *v) {
    Ctx &ctx(v->ctx);
    gtk_list_store_clear(v->tasks);
    size_t cnt(0);
    
    str text_sel(trim(gtk_entry_get_text(GTK_ENTRY(v->text_fld))));
    bool done_sel(gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(v->done_fld)));
    auto peer_sel(get_sel_rec<Peer>(GTK_COMBO_BOX(v->peer_fld)));
    
    for (auto key = ctx.db.tasks_sort.recs.begin();
	 key != ctx.db.tasks_sort.recs.end();
	 key++) {
      auto &rec(db::get(ctx.db.tasks, *key));
      Task tsk(ctx, rec);

      if (tsk.done != done_sel) { continue; }

      if (peer_sel) {
	auto peer_id(*db::get(*peer_sel, ctx.db.peer_id));
	if (tsk.owner_id != peer_id &&
	    tsk.peer_ids.find(peer_id) == tsk.peer_ids.end()) { continue; }
      }      

      if (!text_sel.empty() &&
	  find_ci(tsk.name, text_sel) == str::npos &&
	  find_ci(tsk.info, text_sel) == str::npos) {
	continue;
      }

      Project prj(get_project_id(ctx, tsk.project_id));
      Peer own(get_peer_id(ctx, tsk.owner_id));
      GtkTreeIter iter;
      gtk_list_store_append(v->tasks, &iter);
      gtk_list_store_set(v->tasks, &iter,
			 COL_TASK_PTR, &rec,
			 COL_TASK_CREATED,
			 fmt(tsk.created_at, "%a %b %d, %H:%M").c_str(),
			 COL_TASK_OWNER, own.name.c_str(),
			 COL_TASK_NAME, tsk.name.c_str(),
			 COL_TASK_INFO, tsk.info.c_str(),
			 COL_TASK_PROJECT, prj.name.c_str(),
			 -1);
      cnt++;
    }

    gtk_widget_grab_focus(cnt ? v->lst : v->peer_fld);
  }

  static void on_edit(GtkTreeView *treeview,
		      GtkTreePath *path,
		      GtkTreeViewColumn *col,
		      TaskSearch *v) {
    Ctx &ctx(v->ctx);
    auto rec(get_sel_rec<Task>(GTK_TREE_VIEW(v->lst)));
    CHECK(rec ? true : false, _);
    Task task(ctx, *rec);
    TaskView *fv(new TaskView(task));
    push_view(*fv);
  }
  
  static void on_close(gpointer *_, TaskSearch *v) {
    pop_view(*v);
  }
  
  static void init_peers(TaskSearch &v) {
    GtkTreeIter iter;
    gtk_list_store_append(v.peers, &iter);
    gtk_list_store_set(v.peers, &iter,
		       COL_PEER_PTR, nullptr,
		       COL_PEER_ID, "",
		       COL_PEER_NAME, "",
		       -1);

    for(auto key = v.ctx.db.peers_sort.recs.begin();
	key != v.ctx.db.peers_sort.recs.end();
	key++) {
      auto &rec(db::get(v.ctx.db.peers, *key));
      Peer peer(v.ctx, rec);
      gtk_list_store_append(v.peers, &iter);
      gtk_list_store_set(v.peers, &iter,
			 COL_PEER_PTR, &rec,
			 COL_PEER_ID, to_str(peer.id).c_str(),
			 COL_PEER_NAME, peer.name.c_str(),
			 -1);
    }
  }

  static void init_lst(TaskSearch &v) {
    gtk_widget_set_hexpand(v.lst, true);
    add_col(GTK_TREE_VIEW(v.lst), "Created", COL_TASK_CREATED);
    add_col(GTK_TREE_VIEW(v.lst), "Owner", COL_TASK_OWNER);
    add_col(GTK_TREE_VIEW(v.lst), "Project", COL_TASK_PROJECT, true);
    add_col(GTK_TREE_VIEW(v.lst), "Name", COL_TASK_NAME, true);
    add_col(GTK_TREE_VIEW(v.lst), "Info", COL_TASK_INFO, true);
    g_signal_connect(v.lst, "row-activated", G_CALLBACK(on_edit), &v);
  }
  
  TaskSearch::TaskSearch(Ctx &ctx):
    View(ctx, "Task Search"),
    peers(gtk_list_store_new(3, G_TYPE_POINTER, G_TYPE_STRING, G_TYPE_STRING)),
    tasks(gtk_list_store_new(6, G_TYPE_POINTER,
			     G_TYPE_STRING, G_TYPE_STRING, G_TYPE_STRING,
			     G_TYPE_STRING, G_TYPE_STRING)),
    text_fld(gtk_entry_new()),
    done_fld(gtk_check_button_new_with_label("Done")),
    peer_fld(new_combo_box(GTK_TREE_MODEL(peers))),
    find_btn(gtk_button_new_with_mnemonic("_Find Tasks")),
    lst(gtk_tree_view_new_with_model(GTK_TREE_MODEL(tasks))),
    close_btn(gtk_button_new_with_mnemonic("_Close Search"))
  { }

  void TaskSearch::init() {
    GtkWidget *lbl;

    GtkWidget *frm = gtk_box_new(GTK_ORIENTATION_VERTICAL, 5);
    gtk_box_pack_start(GTK_BOX(panel), frm, false, false, 0);

    lbl = gtk_label_new("Text");
    gtk_widget_set_halign(lbl, GTK_ALIGN_START);
    gtk_container_add(GTK_CONTAINER(frm), lbl);
    GtkWidget *text_box(gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 5));
    gtk_container_add(GTK_CONTAINER(frm), text_box);
    gtk_widget_set_hexpand(text_fld, true);
    gtk_container_add(GTK_CONTAINER(text_box), text_fld);
    gtk_container_add(GTK_CONTAINER(text_box), done_fld);

    init_peers(*this);
    lbl = gtk_label_new("Peer");
    gtk_widget_set_halign(lbl, GTK_ALIGN_START);
    gtk_container_add(GTK_CONTAINER(frm), lbl);
    gtk_widget_set_hexpand(peer_fld, true);
    gtk_container_add(GTK_CONTAINER(frm), peer_fld);

    gtk_widget_set_halign(find_btn, GTK_ALIGN_END);
    gtk_widget_set_margin_top(find_btn, 5);
    g_signal_connect(find_btn, "clicked", G_CALLBACK(on_find), this);
    gtk_container_add(GTK_CONTAINER(frm), find_btn);

    init_lst(*this);
    gtk_box_pack_start(GTK_BOX(panel), lst, true, true, 0);
    lbl = gtk_label_new("Press Return or double-click to edit task");
    gtk_container_add(GTK_CONTAINER(panel), lbl);

    GtkWidget *btns = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 5);
    gtk_widget_set_halign(btns, GTK_ALIGN_END);
    gtk_widget_set_valign(btns, GTK_ALIGN_END);
    gtk_widget_set_margin_top(btns, 10);
    gtk_container_add(GTK_CONTAINER(panel), btns);
        
    g_signal_connect(close_btn, "clicked", G_CALLBACK(on_close), this);
    gtk_container_add(GTK_CONTAINER(btns), close_btn);
    focused = text_fld;
  }
}}
