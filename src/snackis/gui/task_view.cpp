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
    if (pn == 1 && !post_count(v->post_lst)) {
      auto fd(find_feed_id(v->ctx, v->rec.id));
      if (fd) { load(v->post_lst, *fd, now()); }
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

  static GtkWidget *init_general(TaskView &v) {
    GtkWidget *frm(gtk_box_new(GTK_ORIENTATION_VERTICAL, 5));
    gtk_widget_set_margin_top(frm, 5);
    
    gtk_container_add(GTK_CONTAINER(frm), new_label("Project"));
    GtkWidget *project_box(gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 5));
    gtk_container_add(GTK_CONTAINER(frm), project_box);

    v.project_fld.on_change.emplace([&v]() {
	if (v.project_fld.selected) {
	  set_project(v.rec, *v.project_fld.selected);
	}
	
	set_str(GTK_ENTRY(v.tags_fld),
		join(v.rec.tags.begin(), v.rec.tags.end(), ' '));
	auto sel(v.project_fld.selected ? true : false);
	gtk_widget_set_sensitive(v.project_btn, sel);
	refresh(v);
      });
    
    gtk_container_add(GTK_CONTAINER(project_box), v.project_fld.ptr());
    g_signal_connect(v.project_btn, "clicked", G_CALLBACK(on_project), &v);
    gtk_container_add(GTK_CONTAINER(project_box), v.project_btn);

    if (v.rec.project_id != null_uid) {
      select<Project>(v.project_fld, get_project_id(v.ctx, v.rec.project_id));
    }
    
    GtkWidget *lbl;
    lbl = new_label("Name");
    gtk_widget_set_margin_top(lbl, 5);
    gtk_container_add(GTK_CONTAINER(frm), lbl);
    GtkWidget *name_box = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 5);
    gtk_container_add(GTK_CONTAINER(frm), name_box);
    gtk_widget_set_hexpand(v.name_fld, true);
    gtk_container_add(GTK_CONTAINER(name_box), v.name_fld);
    set_str(GTK_ENTRY(v.name_fld), v.rec.name);
    gtk_container_add(GTK_CONTAINER(name_box), v.done_fld);
    gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(v.done_fld), v.rec.done);

    gtk_container_add(GTK_CONTAINER(frm), new_label("Tags"));
    gtk_container_add(GTK_CONTAINER(frm), v.tags_fld);
    set_str(GTK_ENTRY(v.tags_fld), join(v.rec.tags.begin(), v.rec.tags.end(), ' '));
    
    lbl = new_label("Info");
    gtk_widget_set_margin_top(lbl, 5);
    gtk_container_add(GTK_CONTAINER(frm), lbl);
    gtk_container_add(GTK_CONTAINER(frm), gtk_widget_get_parent(v.info_fld));
    set_str(GTK_TEXT_VIEW(v.info_fld), v.rec.info);
    return frm;
  }
  
  TaskView::TaskView(const Task &tsk):
    SharedView<Task>("Task", tsk),
    find_posts_btn(gtk_button_new_with_mnemonic("_Find Posts")),
    post_btn(gtk_button_new_with_mnemonic("New _Post")),
    project_btn(gtk_button_new_with_mnemonic("_View Project")),
    name_fld(gtk_entry_new()),
    done_fld(gtk_check_button_new_with_mnemonic("_Done")),
    tags_fld(gtk_entry_new()),    
    info_fld(new_text_view()),
    project_fld(ctx),
    post_lst(ctx)
  {
    gtk_widget_set_sensitive(find_posts_btn,
			     find_feed_id(ctx, rec.id) ? true : false);
    g_signal_connect(find_posts_btn, "clicked", G_CALLBACK(on_find_posts), this);
    gtk_container_add(GTK_CONTAINER(menu), find_posts_btn);
    g_signal_connect(post_btn, "clicked", G_CALLBACK(on_post), this);
    gtk_container_add(GTK_CONTAINER(menu), post_btn);
    
    GtkWidget *tabs(gtk_notebook_new());
    gtk_widget_set_vexpand(tabs, true);
    g_signal_connect(tabs, "switch-page", G_CALLBACK(on_page), this);
    gtk_container_add(GTK_CONTAINER(fields), tabs);

    gtk_notebook_append_page(GTK_NOTEBOOK(tabs),
			     init_general(*this),
			     gtk_label_new_with_mnemonic("_1 General"));
    
    GtkWidget *lbl(gtk_label_new_with_mnemonic("_2 Post History"));
    if (!find_feed_id(ctx, rec.id)) { gtk_widget_set_sensitive(lbl, false); }
    gtk_notebook_append_page(GTK_NOTEBOOK(tabs),
			     post_lst.ptr(),
			     lbl);
    
    focused = project_fld.search_btn;
    refresh(*this);
  }

  bool TaskView::allow_save() const {
    return project_fld.selected ? true : false;
  }

  bool TaskView::save() {
    rec.name = get_str(GTK_ENTRY(name_fld));
    rec.done = gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(done_fld));
    rec.tags = word_set(get_str(GTK_ENTRY(tags_fld)));    
    rec.info = get_str(GTK_TEXT_VIEW(info_fld));
    db::upsert(ctx.db.tasks, rec);
    return true;
  }
}}
