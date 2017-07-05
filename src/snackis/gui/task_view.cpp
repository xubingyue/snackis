#include <cassert>
#include "snackis/ctx.hpp"
#include "snackis/gui/gui.hpp"
#include "snackis/gui/post_search.hpp"
#include "snackis/gui/post_view.hpp"
#include "snackis/gui/project_view.hpp"
#include "snackis/gui/task_view.hpp"

namespace snackis {
namespace gui {
  enum ProjectCol {COL_PROJECT_PTR=0, COL_PROJECT_ID, COL_PROJECT_NAME};

  static void on_project_sel(gpointer *_, TaskView *v) {
    auto sel(get_sel_rec<Project>(GTK_COMBO_BOX(v->project_fld)) ? true : false);
    gtk_widget_set_sensitive(v->edit_project_btn, sel);
    refresh(*v);
  }

  static void on_edit_project(gpointer *_, TaskView *v) {
    Project prj(v->ctx, *get_sel_rec<Project>(GTK_COMBO_BOX(v->project_fld)));
    ProjectView *fv(new ProjectView(prj));
    push_view(*fv);
  }
  
  void init_projects(TaskView &v) {
    Ctx &ctx(v.ctx);
    size_t cnt(0);
    
    for(auto key = ctx.db.projects_sort.recs.begin();
	key != ctx.db.projects_sort.recs.end();
	key++) {
      auto &rec(db::get(ctx.db.projects, *key));
      Project project(ctx, rec);
      
      if (project.id == v.rec.project_id || project.active) {
	GtkTreeIter iter;
	gtk_list_store_append(v.project_store, &iter);
	gtk_list_store_set(v.project_store, &iter,
			   COL_PROJECT_PTR, &rec,
			   COL_PROJECT_ID, to_str(project.id).c_str(),
			   COL_PROJECT_NAME, project.name.c_str(),
			   -1);
	cnt++;
      }
    }
    
    if (!gtk_combo_box_set_active_id(GTK_COMBO_BOX(v.project_fld),
				     to_str(v.rec.project_id).c_str())) {
      gtk_combo_box_set_active(GTK_COMBO_BOX(v.project_fld), 0);
    }
  }

  static void on_find_posts(gpointer *_, TaskView *v) {
    PostSearch *ps = new PostSearch(v->ctx);
    select<Feed>(ps->feed_fld, get_feed(v->rec));
    push_view(*ps);
  }

  static void on_post(gpointer *_, TaskView *v) {
    db::Trans trans(v->ctx);
    v->save();

    Post post(v->ctx);
    post.feed_id = get_feed(v->rec).id;
    PostView *pv = new PostView(post);
    push_view(*pv);

    db::commit(trans);
  }

  TaskView::TaskView(const Task &rec):
    RecView("Task", rec),
    project_store(gtk_list_store_new(3, G_TYPE_POINTER,
				     G_TYPE_STRING, G_TYPE_STRING)),
    find_posts_btn(gtk_button_new_with_mnemonic("_Find Posts")),
    post_btn(gtk_button_new_with_mnemonic("New _Post")),
    project_fld(new_combo_box(GTK_TREE_MODEL(project_store))),
    edit_project_btn(gtk_button_new_with_mnemonic("_Edit Project")),
    name_fld(gtk_entry_new()),
    done_fld(gtk_check_button_new_with_mnemonic("_Done")),
    info_fld(new_text_view()),
    queue_lst(ctx, "Queue", this->rec.queue_ids)    
  {
    GtkWidget *lbl;

    GtkWidget *btns = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 5);
    gtk_widget_set_margin_bottom(btns, 5);
    gtk_container_add(GTK_CONTAINER(fields), btns);
    g_signal_connect(find_posts_btn, "clicked", G_CALLBACK(on_find_posts), this);
    gtk_container_add(GTK_CONTAINER(btns), find_posts_btn);
    g_signal_connect(post_btn, "clicked", G_CALLBACK(on_post), this);
    gtk_container_add(GTK_CONTAINER(btns), post_btn);
    
    init_projects(*this);
    lbl = gtk_label_new("Project");
    gtk_widget_set_halign(lbl, GTK_ALIGN_START);
    gtk_container_add(GTK_CONTAINER(fields), lbl);
    GtkWidget *project_box = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 5);
    gtk_container_add(GTK_CONTAINER(fields), project_box);
    gtk_widget_set_hexpand(project_fld, true);
    g_signal_connect(project_fld, "changed", G_CALLBACK(on_project_sel), this);
    gtk_container_add(GTK_CONTAINER(project_box), project_fld);
    g_signal_connect(edit_project_btn, "clicked", G_CALLBACK(on_edit_project), this);
    gtk_container_add(GTK_CONTAINER(project_box), edit_project_btn);
    on_project_sel(nullptr, this);
    
    lbl = gtk_label_new("Name");
    gtk_widget_set_halign(lbl, GTK_ALIGN_START);
    gtk_widget_set_margin_top(lbl, 5);
    gtk_container_add(GTK_CONTAINER(fields), lbl);
    GtkWidget *name_box = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 5);
    gtk_container_add(GTK_CONTAINER(fields), name_box);
    gtk_widget_set_hexpand(name_fld, true);
    gtk_container_add(GTK_CONTAINER(name_box), name_fld);
    set_str(GTK_ENTRY(name_fld), rec.name);
    gtk_container_add(GTK_CONTAINER(name_box), done_fld);
    gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(done_fld), rec.done);

    lbl = gtk_label_new("Info");
    gtk_widget_set_halign(lbl, GTK_ALIGN_START);
    gtk_container_add(GTK_CONTAINER(fields), lbl);
    gtk_container_add(GTK_CONTAINER(fields), gtk_widget_get_parent(info_fld));
    set_str(GTK_TEXT_VIEW(info_fld), rec.info);

    load(queue_lst);
    gtk_widget_set_margin_top(queue_lst.ptr(), 5);
    gtk_container_add(GTK_CONTAINER(fields), queue_lst.ptr());
    
    focused = project_fld;
    refresh(*this);
  }

  bool TaskView::allow_save() const {
    return rec.owner_id == whoami(ctx).id &&
      get_sel_rec<Project>(GTK_COMBO_BOX(project_fld));
  }

  bool TaskView::save() {
    Project prj(ctx, *get_sel_rec<Project>(GTK_COMBO_BOX(project_fld)));
    set_project(rec, prj);
    rec.name = get_str(GTK_ENTRY(name_fld));
    rec.info = get_str(GTK_TEXT_VIEW(info_fld));
    rec.done = gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(done_fld));
    db::upsert(ctx.db.tasks, rec);
    return true;
  }
}}
