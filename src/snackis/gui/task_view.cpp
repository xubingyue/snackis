#include <cassert>
#include "snackis/ctx.hpp"
#include "snackis/gui/gui.hpp"
#include "snackis/gui/post_search.hpp"
#include "snackis/gui/post_view.hpp"
#include "snackis/gui/project_view.hpp"
#include "snackis/gui/task_view.hpp"

namespace snackis {
namespace gui {
  static void on_edit_project(gpointer *_, TaskView *v) {
    CHECK(v->project_fld.selected != nullopt, _);
    ProjectView *fv(new ProjectView(*v->project_fld.selected));
    push_view(*fv);
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
    find_posts_btn(gtk_button_new_with_mnemonic("_Find Posts")),
    post_btn(gtk_button_new_with_mnemonic("New _Post")),
    edit_project_btn(gtk_button_new_with_mnemonic("_Edit Project")),
    name_fld(gtk_entry_new()),
    done_fld(gtk_check_button_new_with_mnemonic("_Done")),
    info_fld(new_text_view()),
    project_fld(ctx),
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
    
    lbl = gtk_label_new("Project");
    gtk_widget_set_halign(lbl, GTK_ALIGN_START);
    gtk_container_add(GTK_CONTAINER(fields), lbl);
    GtkWidget *project_box = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 5);
    gtk_container_add(GTK_CONTAINER(fields), project_box);

    project_fld.on_change.emplace([this]() {
	auto sel(project_fld.selected ? true : false);
	gtk_widget_set_sensitive(edit_project_btn, sel);
	refresh(*this);
      });
    
    gtk_container_add(GTK_CONTAINER(project_box), project_fld.ptr());
    g_signal_connect(edit_project_btn, "clicked", G_CALLBACK(on_edit_project), this);
    gtk_container_add(GTK_CONTAINER(project_box), edit_project_btn);

    if (rec.project_id != null_uid) {
      select<Project>(project_fld, get_project_id(ctx, rec.project_id));
    }
    
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
    
    focused = project_fld.ptr();
    refresh(*this);
  }

  bool TaskView::allow_save() const {
    return rec.owner_id == whoami(ctx).id && project_fld.selected;
  }

  bool TaskView::save() {
    set_project(rec, *project_fld.selected);
    rec.name = get_str(GTK_ENTRY(name_fld));
    rec.info = get_str(GTK_TEXT_VIEW(info_fld));
    rec.done = gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(done_fld));
    db::upsert(ctx.db.tasks, rec);
    return true;
  }
}}
