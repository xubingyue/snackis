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
  enum TaskCol {COL_TASK_PTR=0, COL_TASK_ID, COL_TASK_CREATED, COL_TASK_OWNER,
		COL_TASK_PROJECT, COL_TASK_NAME, COL_TASK_INFO};

  static opt<Task> add_task(TaskSearch &v, const db::Rec<Task> &rec) {
    Ctx &ctx(v.ctx);
    Task tsk(ctx, rec);

    str id_sel(trim(gtk_entry_get_text(GTK_ENTRY(v.id_fld))));

    if (!id_sel.empty() && find_ci(id_str(tsk), id_sel) == str::npos) {
      return nullopt;
    }

    str text_sel(trim(gtk_entry_get_text(GTK_ENTRY(v.text_fld))));

    if (!text_sel.empty() &&
	find_ci(tsk.name, text_sel) == str::npos &&
	find_ci(tsk.info, text_sel) == str::npos) {
      return nullopt;
    }

    bool done_sel(gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(v.done_fld)));

    if (tsk.done != done_sel) { return nullopt; }

    auto project_sel(get_sel_rec<Project>(GTK_COMBO_BOX(v.project_fld)));

    if (project_sel) {
      auto id (*db::get(*project_sel, ctx.db.project_id));
      if (tsk.project_id != id) { return nullopt; }
    }

    auto peer_sel(v.peer_fld.selected);

    if (peer_sel) {
      if (tsk.owner_id != peer_sel->id &&
	  tsk.peer_ids.find(peer_sel->id) == tsk.peer_ids.end()) { return nullopt; }
    }      

    Project prj(get_project_id(ctx, tsk.project_id));
    Peer own(get_peer_id(ctx, tsk.owner_id));
    GtkTreeIter iter;
    gtk_list_store_append(v.store, &iter);
    gtk_list_store_set(v.store, &iter,
		       COL_TASK_PTR, &rec,
		       COL_TASK_ID, id_str(tsk).c_str(),
		       COL_TASK_CREATED,
		       fmt(tsk.created_at, "%a %b %d, %H:%M").c_str(),
		       COL_TASK_OWNER, own.name.c_str(),
		       COL_TASK_NAME, tsk.name.c_str(),
		       COL_TASK_INFO, tsk.info.c_str(),
		       COL_TASK_PROJECT, prj.name.c_str(),
		       -1);

    return tsk;
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

  static void edit(Ctx &ctx, const db::Rec<Task> &rec) {
    Task task(ctx, rec);
    TaskView *fv(new TaskView(task));
    push_view(*fv);
  }

  TaskSearch::TaskSearch(Ctx &ctx):
    SearchView<Task>(ctx,
		     "Task",
		     gtk_list_store_new(7, G_TYPE_POINTER,
					G_TYPE_STRING,
					G_TYPE_STRING,
					G_TYPE_STRING,
					G_TYPE_STRING,
					G_TYPE_STRING,
					G_TYPE_STRING),
		     [&ctx](auto &rec) { edit(ctx, rec); }),
    project_store(gtk_list_store_new(3, G_TYPE_POINTER,
				     G_TYPE_STRING,
				     G_TYPE_STRING)),
    queue_store(gtk_list_store_new(3, G_TYPE_POINTER,
				   G_TYPE_STRING,
				   G_TYPE_STRING)),
    id_fld(gtk_entry_new()),
    text_fld(gtk_entry_new()),
    done_fld(gtk_check_button_new_with_label("Done")),
    project_fld(new_combo_box(GTK_TREE_MODEL(project_store))),
    queue_fld(new_combo_box(GTK_TREE_MODEL(queue_store))),
    peer_fld(ctx)
  { }

  void TaskSearch::init() {
    SearchView<Task>::init();
    GtkWidget *lbl;

    GtkWidget *top_box = gtk_grid_new();
    gtk_grid_set_row_spacing(GTK_GRID(top_box), 5);
    gtk_grid_set_column_spacing(GTK_GRID(top_box), 5);
    gtk_container_add(GTK_CONTAINER(fields), top_box);
    
    lbl = gtk_label_new("Id");
    gtk_widget_set_halign(lbl, GTK_ALIGN_START);
    gtk_grid_attach(GTK_GRID(top_box), lbl, 0, 0, 1, 1);
    gtk_grid_attach(GTK_GRID(top_box), id_fld, 0, 1, 1, 1);

    lbl = gtk_label_new("Text");
    gtk_widget_set_halign(lbl, GTK_ALIGN_START);
    gtk_grid_attach(GTK_GRID(top_box), lbl, 1, 0, 1, 1);
    gtk_widget_set_hexpand(text_fld, true);
    gtk_grid_attach(GTK_GRID(top_box), text_fld, 1, 1, 1, 1);
    gtk_grid_attach(GTK_GRID(top_box), done_fld, 2, 1, 1, 1);

    GtkWidget *frm = gtk_grid_new();
    gtk_grid_set_row_spacing(GTK_GRID(frm), 5);
    gtk_grid_set_column_spacing(GTK_GRID(frm), 5);
    gtk_container_add(GTK_CONTAINER(fields), frm);

    int row = 0;
    init_projects(*this);
    lbl = gtk_label_new("Project");
    gtk_widget_set_halign(lbl, GTK_ALIGN_START);
    gtk_grid_attach(GTK_GRID(frm), lbl, 0, row, 1, 1);
    gtk_widget_set_hexpand(project_fld, true);
    gtk_grid_attach(GTK_GRID(frm), project_fld, 0, row+1, 1, 1);

    init_queues(*this);
    lbl = gtk_label_new("Queue");
    gtk_widget_set_halign(lbl, GTK_ALIGN_START);
    gtk_grid_attach(GTK_GRID(frm), lbl, 1, row, 1, 1);
    gtk_widget_set_hexpand(queue_fld, true);
    gtk_grid_attach(GTK_GRID(frm), queue_fld, 1, row+1, 1, 1);

    row += 2;
    lbl = gtk_label_new("Peer");
    gtk_widget_set_halign(lbl, GTK_ALIGN_START);
    gtk_grid_attach(GTK_GRID(frm), lbl, 0, row, 2, 1);
    gtk_grid_attach(GTK_GRID(frm), peer_fld.ptr(), 0, row+1, 2, 1);

    add_col(GTK_TREE_VIEW(list), "Id", COL_TASK_ID);
    add_col(GTK_TREE_VIEW(list), "Created", COL_TASK_CREATED);
    add_col(GTK_TREE_VIEW(list), "Owner", COL_TASK_OWNER);
    add_col(GTK_TREE_VIEW(list), "Project", COL_TASK_PROJECT);
    add_col(GTK_TREE_VIEW(list), "Name", COL_TASK_NAME);
    add_col(GTK_TREE_VIEW(list), "Info", COL_TASK_INFO);
    focused = text_fld;
  }

  void TaskSearch::find() {
    auto queue_sel(get_sel_rec<Queue>(GTK_COMBO_BOX(queue_fld)));
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
	add_task(*this, rec);
	cnt++;
      }
    } else {
    
      for (auto i = ctx.db.tasks_sort.recs.begin();
	   i != ctx.db.tasks_sort.recs.end();
	   i++) {
	auto &rec(db::get(ctx.db.tasks, *i));
	auto tsk(add_task(*this, rec));
	cnt++;
      }
    }
    
    gtk_widget_grab_focus(cnt ? list : text_fld);
  }
}}
