#include <cassert>
#include "snackis/ctx.hpp"
#include "snackis/gui/gui.hpp"
#include "snackis/gui/feed_view.hpp"
#include "snackis/gui/post_search.hpp"
#include "snackis/gui/post_view.hpp"
#include "snackis/gui/project_view.hpp"
#include "snackis/gui/task_search.hpp"
#include "snackis/gui/task_view.hpp"

namespace snackis {
namespace gui {
  static void on_find_tasks(gpointer *_, ProjectView *v) {
    TaskSearch *ts = new TaskSearch(v->ctx);
    select<Project>(ts->project_fld, v->rec);
    push_view(ts);
  }

  static void on_new_task(gpointer *_, ProjectView *v) {
    Task tsk(v->ctx);
    set_project(tsk, v->rec);
    push_view(new TaskView(tsk));
  }

  static void on_find_posts(gpointer *_, ProjectView *v) {
    PostSearch *ps = new PostSearch(v->ctx);
    select<Feed>(ps->feed_fld, get_feed(v->rec));
    push_view(ps);
  }

  static void on_post(gpointer *_, ProjectView *v) {
    db::Trans trans(v->ctx);
    TRY(try_save);
    v->save();
    
    if (try_save.errors.empty()) {
      db::commit(trans, fmt("Saved Project %0", id_str(v->rec)));
      Post post(v->ctx);
      post.feed_id = get_feed(v->rec).id;
      push_view(new PostView(post));
    }
  }

  ProjectView::ProjectView(const Project &rec):
    SharedView<Project>("Project", rec),
    find_tasks_btn(gtk_button_new_with_mnemonic("_Find Tasks")),
    new_task_btn(gtk_button_new_with_mnemonic("New _Task")),
    find_posts_btn(gtk_button_new_with_mnemonic("Find Posts")),
    post_btn(gtk_button_new_with_mnemonic("New _Post")),
    name_fld(gtk_entry_new()),
    tags_fld(gtk_entry_new()),
    active_fld(gtk_check_button_new_with_label("Active")),    
    info_fld(new_text_view()),
    peer_lst(ctx, "Peer", this->rec.peer_ids)
  {
    GtkWidget *lbl;

    g_signal_connect(find_tasks_btn, "clicked", G_CALLBACK(on_find_tasks), this);
    gtk_container_add(GTK_CONTAINER(menu), find_tasks_btn);
    g_signal_connect(new_task_btn, "clicked", G_CALLBACK(on_new_task), this);
    gtk_container_add(GTK_CONTAINER(menu), new_task_btn);
    gtk_widget_set_sensitive(find_posts_btn,
			     find_feed_id(ctx, rec.id) ? true : false);
    g_signal_connect(find_posts_btn, "clicked", G_CALLBACK(on_find_posts), this);
    gtk_container_add(GTK_CONTAINER(menu), find_posts_btn);
    g_signal_connect(post_btn, "clicked", G_CALLBACK(on_post), this);
    gtk_container_add(GTK_CONTAINER(menu), post_btn);
    
    lbl = gtk_label_new("Name");
    gtk_widget_set_halign(lbl, GTK_ALIGN_START);
    gtk_container_add(GTK_CONTAINER(fields), lbl);
    GtkWidget *name_box = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 5);
    gtk_container_add(GTK_CONTAINER(fields), name_box);
    gtk_widget_set_hexpand(name_fld, true);    
    gtk_container_add(GTK_CONTAINER(name_box), name_fld);
    gtk_entry_set_text(GTK_ENTRY(name_fld), rec.name.c_str());
    gtk_container_add(GTK_CONTAINER(name_box), active_fld);
    gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(active_fld), rec.active);

    lbl = gtk_label_new("Tags");
    gtk_widget_set_halign(lbl, GTK_ALIGN_START);
    gtk_container_add(GTK_CONTAINER(fields), lbl);
    gtk_widget_set_hexpand(tags_fld, true);
    gtk_container_add(GTK_CONTAINER(fields), tags_fld);    
    gtk_entry_set_text(GTK_ENTRY(tags_fld),
		       join(rec.tags.begin(), rec.tags.end(), ' ').c_str());
    
    lbl = gtk_label_new("Info");
    gtk_widget_set_halign(lbl, GTK_ALIGN_START);
    gtk_widget_set_margin_top(lbl, 5);    
    gtk_container_add(GTK_CONTAINER(fields), lbl);
    gtk_container_add(GTK_CONTAINER(fields), gtk_widget_get_parent(info_fld));
    set_str(GTK_TEXT_VIEW(info_fld), rec.info);
    
    gui::load(peer_lst);
    gtk_widget_set_margin_top(peer_lst.ptr(), 5);
    gtk_container_add(GTK_CONTAINER(fields), peer_lst.ptr());
    
    focused = name_fld;
  }

  bool ProjectView::save() {
    rec.name = gtk_entry_get_text(GTK_ENTRY(name_fld));
    rec.tags = word_set(get_str(GTK_ENTRY(tags_fld)));
    rec.info = get_str(GTK_TEXT_VIEW(info_fld));
    db::upsert(ctx.db.projects, rec);
    return true;
  }
}}
