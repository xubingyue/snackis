#include <cassert>
#include "snackis/ctx.hpp"
#include "snackis/gui/gui.hpp"
#include "snackis/gui/task_search.hpp"
#include "snackis/gui/task_view.hpp"

namespace snackis {
namespace gui {
  enum ProjectCol {COL_PROJECT_PTR=0, COL_PROJECT_ID, COL_PROJECT_NAME};
  enum QueueCol {COL_QUEUE_PTR=0, COL_QUEUE_ID, COL_QUEUE_NAME};
  enum PeerCol {COL_PEER_PTR=0, COL_PEER_ID, COL_PEER_NAME};
  enum TaskCol {COL_TASK_PTR=0, COL_TASK_CREATED, COL_TASK_OWNER,
		COL_TASK_PROJECT, COL_TASK_NAME, COL_TASK_INFO};

  static opt<Task> add_task(TaskSearch &v, const db::Rec<Task> &rec) {
    Ctx &ctx(v.ctx);
    Task tsk(ctx, rec);

    bool done_sel(gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(v.done_fld)));

    if (tsk.done != done_sel) { return nullopt; }

    auto project_sel(get_sel_rec<Project>(GTK_COMBO_BOX(v.project_fld)));

    if (project_sel) {
      auto id (*db::get(*project_sel, ctx.db.project_id));
      if (tsk.project_id != id) { return nullopt; }
    }

    auto peer_sel(get_sel_rec<Peer>(GTK_COMBO_BOX(v.peer_fld)));

    if (peer_sel) {
      auto peer_id(*db::get(*peer_sel, ctx.db.peer_id));
      if (tsk.owner_id != peer_id &&
	  tsk.peer_ids.find(peer_id) == tsk.peer_ids.end()) { return nullopt; }
    }      

    str text_sel(trim(gtk_entry_get_text(GTK_ENTRY(v.text_fld))));

    if (!text_sel.empty() &&
	find_ci(tsk.name, text_sel) == str::npos &&
	find_ci(tsk.info, text_sel) == str::npos) {
      return nullopt;
    }

    Project prj(get_project_id(ctx, tsk.project_id));
    Peer own(get_peer_id(ctx, tsk.owner_id));
    GtkTreeIter iter;
    gtk_list_store_append(v.tasks, &iter);
    gtk_list_store_set(v.tasks, &iter,
		       COL_TASK_PTR, &rec,
		       COL_TASK_CREATED,
		       fmt(tsk.created_at, "%a %b %d, %H:%M").c_str(),
		       COL_TASK_OWNER, own.name.c_str(),
		       COL_TASK_NAME, tsk.name.c_str(),
		       COL_TASK_INFO, tsk.info.c_str(),
		       COL_TASK_PROJECT, prj.name.c_str(),
		       -1);

    return tsk;
  }
  
  static void on_find(gpointer *_, TaskSearch *v) {
    Ctx &ctx(v->ctx);
    gtk_list_store_clear(v->tasks);
    auto queue_sel(get_sel_rec<Queue>(GTK_COMBO_BOX(v->queue_fld)));
    size_t cnt(0);    
    
    if (queue_sel) {
      Queue q(ctx, *queue_sel);
      db::Rec<QueueTask> key;
      db::set(key, ctx.db.queue_task_queue_id, q.id);
      for (auto i = ctx.db.queue_tasks.recs.lower_bound(key);
	   i != ctx.db.queue_tasks.recs.end() &&
	     *db::get(*i, ctx.db.queue_task_queue_id) == q.id;
	   i++) {
	db::Rec<Task> rec_key;
	db::set(rec_key, ctx.db.task_id, *db::get(*i, ctx.db.queue_task_id));
	auto &rec(db::get(ctx.db.tasks, rec_key));
	add_task(*v, rec);
	cnt++;
      }
    } else {
    
      for (auto i = ctx.db.tasks_sort.recs.begin();
	   i != ctx.db.tasks_sort.recs.end();
	   i++) {
	auto &rec(db::get(ctx.db.tasks, *i));
	auto tsk(add_task(*v, rec));
	cnt++;
      }
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
  
  static void init_projects(TaskSearch &v) {
    GtkTreeIter iter;
    gtk_list_store_append(v.project_store, &iter);
    gtk_list_store_set(v.project_store, &iter,
		       COL_PROJECT_PTR, nullptr,
		       COL_PROJECT_ID, "",
		       COL_PROJECT_NAME, "",
		       -1);

    for(auto key = v.ctx.db.projects_sort.recs.begin();
	key != v.ctx.db.projects_sort.recs.end();
	key++) {
      auto &rec(db::get(v.ctx.db.projects, *key));
      Project prj(v.ctx, rec);
      gtk_list_store_append(v.project_store, &iter);
      gtk_list_store_set(v.project_store, &iter,
			 COL_PROJECT_PTR, &rec,
			 COL_PROJECT_ID, to_str(prj.id).c_str(),
			 COL_PROJECT_NAME, prj.name.c_str(),
			 -1);
    }

    if (v.project) {
      gtk_combo_box_set_active_id(GTK_COMBO_BOX(v.project_fld),
				  to_str(v.project->id).c_str());
    } else {
      gtk_combo_box_set_active(GTK_COMBO_BOX(v.project_fld), 0);
    }    
  }

  static void init_queues(TaskSearch &v) {
    GtkTreeIter iter;
    gtk_list_store_append(v.queue_store, &iter);
    gtk_list_store_set(v.queue_store, &iter,
		       COL_QUEUE_PTR, nullptr,
		       COL_QUEUE_ID, "",
		       COL_QUEUE_NAME, "",
		       -1);

    for(auto key = v.ctx.db.queues_sort.recs.begin();
	key != v.ctx.db.queues_sort.recs.end();
	key++) {
      auto &rec(db::get(v.ctx.db.queues, *key));
      Queue q(v.ctx, rec);
      gtk_list_store_append(v.queue_store, &iter);
      gtk_list_store_set(v.queue_store, &iter,
			 COL_QUEUE_PTR, &rec,
			 COL_QUEUE_ID, to_str(q.id).c_str(),
			 COL_QUEUE_NAME, q.name.c_str(),
			 -1);
    }

    if (v.queue) {
      gtk_combo_box_set_active_id(GTK_COMBO_BOX(v.queue_fld),
				  to_str(v.queue->id).c_str());
    } else {
      gtk_combo_box_set_active(GTK_COMBO_BOX(v.queue_fld), 0);
    }    
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

    if (v.peer) {
      gtk_combo_box_set_active_id(GTK_COMBO_BOX(v.peer_fld),
				  to_str(v.peer->id).c_str());
    } else {
      gtk_combo_box_set_active(GTK_COMBO_BOX(v.peer_fld), 0);
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
    project_store(gtk_list_store_new(3, G_TYPE_POINTER,
				     G_TYPE_STRING, G_TYPE_STRING)),
    queue_store(gtk_list_store_new(3, G_TYPE_POINTER,
				   G_TYPE_STRING, G_TYPE_STRING)),
    peers(gtk_list_store_new(3, G_TYPE_POINTER, G_TYPE_STRING, G_TYPE_STRING)),
    tasks(gtk_list_store_new(6, G_TYPE_POINTER,
			     G_TYPE_STRING, G_TYPE_STRING, G_TYPE_STRING,
			     G_TYPE_STRING, G_TYPE_STRING)),
    text_fld(gtk_entry_new()),
    done_fld(gtk_check_button_new_with_label("Done")),
    project_fld(new_combo_box(GTK_TREE_MODEL(project_store))),
    queue_fld(new_combo_box(GTK_TREE_MODEL(queue_store))),
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

    init_projects(*this);
    lbl = gtk_label_new("Project");
    gtk_widget_set_halign(lbl, GTK_ALIGN_START);
    gtk_container_add(GTK_CONTAINER(frm), lbl);
    gtk_widget_set_hexpand(project_fld, true);
    gtk_container_add(GTK_CONTAINER(frm), project_fld);

    init_queues(*this);
    lbl = gtk_label_new("Queue");
    gtk_widget_set_halign(lbl, GTK_ALIGN_START);
    gtk_container_add(GTK_CONTAINER(frm), lbl);
    gtk_widget_set_hexpand(queue_fld, true);
    gtk_container_add(GTK_CONTAINER(frm), queue_fld);

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
