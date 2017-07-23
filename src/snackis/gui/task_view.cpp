#include <cassert>
#include "snackis/ctx.hpp"
#include "snackis/gui/gui.hpp"
#include "snackis/gui/post_search.hpp"
#include "snackis/gui/post_view.hpp"
#include "snackis/gui/project_view.hpp"
#include "snackis/gui/task_view.hpp"

namespace snackis {
namespace gui {
  static void on_page(GtkNotebook *w, GtkWidget *p, guint pn, TaskView *v) {
    switch (pn) {
    case 1:
      load(v->peer_lst);
      break;
    case 2: {
      auto fd(find_feed_id(v->ctx, v->rec.id));
      if (fd) { load(v->post_lst, *fd, now()); }
      break;
    }
    }
  }

  static void on_project(gpointer *_, TaskView *v) {
    CHECK(v->project_fld.selected != nullopt, _);
    ProjectView *fv(new ProjectView(*v->project_fld.selected));
    push_view(fv);
  }
  
  static void on_find_posts(gpointer *_, TaskView *v) {
    PostSearch *ps = new PostSearch(v->ctx);
    select<Feed>(ps->feed_fld, get_feed(v->rec));
    push_view(ps);
  }

  static void on_post(gpointer *_, TaskView *v) {
    db::Trans trans(v->ctx);
    TRY(try_save);
    v->save();

    if (try_save.errors.empty()) {
      db::commit(trans, fmt("Saved Task %0", id_str(v->rec)));
      Post post(v->ctx);
      post.feed_id = get_feed(v->rec).id;
      push_view(new PostView(post));
    }
  }

  static void on_task(gpointer *_, TaskView *v) {
    Task task(v->ctx);
    task.project_id = v->rec.project_id;
    push_view(new TaskView(task));
  }

  static GtkWidget *init_general(TaskView &v) {
    auto me(whoamid(v.ctx));
    GtkWidget *frm(gtk_box_new(GTK_ORIENTATION_VERTICAL, 5));
    gtk_widget_set_margin_top(frm, 5);
    
    gtk_container_add(GTK_CONTAINER(frm), new_label("Project"));
    GtkWidget *project_box(gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 5));
    gtk_container_add(GTK_CONTAINER(frm), project_box);

    v.project_fld.on_change.emplace([&v]() {
	if (v.project_fld.selected) {
	  set_project(v.rec, *v.project_fld.selected);
	  load(v.peer_lst);
	  set_str(GTK_ENTRY(v.tags_fld),
		  join(v.rec.tags.begin(), v.rec.tags.end(), ' '));
	  gtk_widget_set_sensitive(v.project_btn, true);
	} else {
	  gtk_widget_set_sensitive(v.project_btn, false);
	}
	
	refresh(v);
      });
    
    gtk_widget_set_sensitive(v.project_fld.ptr(), v.rec.owner_id == me);
    gtk_container_add(GTK_CONTAINER(project_box), v.project_fld.ptr());
    g_signal_connect(v.project_btn, "clicked", G_CALLBACK(on_project), &v);
    gtk_container_add(GTK_CONTAINER(project_box), v.project_btn);
    
    GtkWidget *name_box(new_grid());
    gtk_widget_set_margin_top(name_box, 5);
    gtk_grid_set_column_homogeneous(GTK_GRID(name_box), true);
    gtk_container_add(GTK_CONTAINER(frm), name_box);

    gtk_grid_attach(GTK_GRID(name_box), new_label("Name"), 0, 0, 2, 1);
    gtk_widget_set_hexpand(v.name_fld, true);
    gtk_editable_set_editable(GTK_EDITABLE(v.name_fld), v.rec.owner_id == me);
    gtk_grid_attach(GTK_GRID(name_box), v.name_fld, 0, 1, 2, 1);		    
  
    gtk_grid_attach(GTK_GRID(name_box), new_label("Prio"), 2, 0, 1, 1);
    set_width(GTK_ENTRY(v.prio_fld), 3);
    gtk_widget_set_halign(v.prio_fld, GTK_ALIGN_START);
    gtk_grid_attach(GTK_GRID(name_box), v.prio_fld, 2, 1, 1, 1);		    
    gtk_widget_set_halign(v.done_fld, GTK_ALIGN_END);
    gtk_widget_set_sensitive(v.done_fld, v.rec.owner_id == me);
    gtk_grid_attach(GTK_GRID(name_box), v.done_fld, 3, 1, 1, 1);		    

    gtk_container_add(GTK_CONTAINER(frm), new_label("Tags"));
    gtk_container_add(GTK_CONTAINER(frm), v.tags_fld);
    
    gtk_container_add(GTK_CONTAINER(frm), new_label("Info"));
    gtk_text_view_set_editable(GTK_TEXT_VIEW(v.info_fld), v.rec.owner_id == me);
    gtk_container_add(GTK_CONTAINER(frm), gtk_widget_get_parent(v.info_fld));
    return frm;
  }
  
  TaskView::TaskView(const Task &tsk):
    SharedView<Task>("Task", tsk),
    task_btn(gtk_button_new_with_mnemonic("New _Task")),
    find_posts_btn(gtk_button_new_with_mnemonic("_Find Posts")),
    post_btn(gtk_button_new_with_mnemonic("New _Post")),
    project_btn(gtk_button_new_with_mnemonic("_View Project")),
    name_fld(gtk_entry_new()),
    prio_fld(gtk_entry_new()),
    done_fld(gtk_check_button_new_with_mnemonic("_Done")),
    tags_fld(gtk_entry_new()),    
    info_fld(new_text_view()),
    project_fld(ctx),
    peer_lst(ctx, "Peer", this->rec.peer_ids),
    post_lst(ctx)
  {
    
    g_signal_connect(task_btn, "clicked", G_CALLBACK(on_task), this);
    gtk_container_add(GTK_CONTAINER(menu), task_btn);
    gtk_widget_set_sensitive(find_posts_btn,
			     find_feed_id(ctx, rec.id) ? true : false);
    g_signal_connect(find_posts_btn, "clicked", G_CALLBACK(on_find_posts), this);
    gtk_container_add(GTK_CONTAINER(menu), find_posts_btn);
    g_signal_connect(post_btn, "clicked", G_CALLBACK(on_post), this);
    gtk_container_add(GTK_CONTAINER(menu), post_btn);

    auto me(whoamid(ctx));
    GtkWidget *tabs(gtk_notebook_new());
    gtk_widget_set_vexpand(tabs, true);
    g_signal_connect(tabs, "switch-page", G_CALLBACK(on_page), this);
    gtk_container_add(GTK_CONTAINER(fields), tabs);

    gtk_notebook_append_page(GTK_NOTEBOOK(tabs),
			     init_general(*this),
			     gtk_label_new_with_mnemonic("_1 General"));
    
    if (rec.owner_id != me) { set_read_only(peer_lst); }
    
    gtk_notebook_append_page(GTK_NOTEBOOK(tabs),
			     peer_lst.ptr(),
			     gtk_label_new_with_mnemonic("_2 Peers"));

    GtkWidget *lbl(gtk_label_new_with_mnemonic("_3 Post History"));
    if (!find_feed_id(ctx, rec.id)) { gtk_widget_set_sensitive(lbl, false); }
    gtk_notebook_append_page(GTK_NOTEBOOK(tabs),
			     post_lst.ptr(),
			     lbl);
    
    focused = (rec.owner_id == me)
      ? project_fld.search_btn
      : name_fld;
    
    refresh(*this);
  }

  bool TaskView::allow_save() const {
    return project_fld.selected ? true : false;
  }

  void TaskView::load() {
    if (rec.project_id != null_uid) {
      select<Project>(project_fld, get_project_id(ctx, rec.project_id));
    }
    
    set_str(GTK_ENTRY(name_fld), rec.name);
    set_str(GTK_ENTRY(prio_fld), to_str(rec.prio));
    gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(done_fld), rec.done);
    set_str(GTK_ENTRY(tags_fld), join(rec.tags.begin(), rec.tags.end(), ' '));
    set_str(GTK_TEXT_VIEW(info_fld), rec.info);
  }
  
  bool TaskView::save() {
    rec.name = get_str(GTK_ENTRY(name_fld));
    rec.prio = to_int64(get_str(GTK_ENTRY(prio_fld)));
    rec.done = gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(done_fld));
    rec.tags = word_set(get_str(GTK_ENTRY(tags_fld)));    
    rec.info = get_str(GTK_TEXT_VIEW(info_fld));
    db::upsert(ctx.db.tasks, rec);
    return true;
  }
}}
