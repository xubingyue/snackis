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
  static void on_page(GtkNotebook *w, GtkWidget *p, guint pn, ProjectView *v) {
    switch (pn) {
    case 1:
      if (!rec_count(v->peer_lst)) {
	load(v->peer_lst);
      }
      
      break;
    case 2:
      if (!post_count(v->post_lst)) {
	auto fd(find_feed_id(v->ctx, v->rec.id));
	if (fd) { load(v->post_lst, *fd, now()); }
      }
      
      break;
    }
  }

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

  static GtkWidget *init_general(ProjectView &v) {
    auto &me(whoami(v.ctx));
    GtkWidget *frm(gtk_box_new(GTK_ORIENTATION_VERTICAL, 5));
    gtk_widget_set_margin_top(frm, 5);

    gtk_container_add(GTK_CONTAINER(frm), new_label("Name"));
    GtkWidget *name_box(gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 5));
    gtk_container_add(GTK_CONTAINER(frm), name_box);
    gtk_widget_set_hexpand(v.name_fld, true);    
    gtk_editable_set_editable(GTK_EDITABLE(v.name_fld), v.rec.owner_id == me.id);    
    gtk_container_add(GTK_CONTAINER(name_box), v.name_fld);
    gtk_entry_set_text(GTK_ENTRY(v.name_fld), v.rec.name.c_str());
    gtk_widget_set_sensitive(v.active_fld, v.rec.owner_id == me.id);    
    gtk_container_add(GTK_CONTAINER(name_box), v.active_fld);
    gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(v.active_fld), v.rec.active);

    gtk_container_add(GTK_CONTAINER(frm), new_label("Tags"));
    gtk_widget_set_hexpand(v.tags_fld, true);
    gtk_container_add(GTK_CONTAINER(frm), v.tags_fld);    
    gtk_entry_set_text(GTK_ENTRY(v.tags_fld),
		       join(v.rec.tags.begin(), v.rec.tags.end(), ' ').c_str());
    
    GtkWidget *l(new_label("Info"));
    gtk_widget_set_margin_top(l, 5);    
    gtk_container_add(GTK_CONTAINER(frm), l);
    gtk_text_view_set_editable(GTK_TEXT_VIEW(v.info_fld), v.rec.owner_id == me.id);
    gtk_container_add(GTK_CONTAINER(frm), gtk_widget_get_parent(v.info_fld));
    set_str(GTK_TEXT_VIEW(v.info_fld), v.rec.info);

    return frm;
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
    peer_lst(ctx, "Peer", this->rec.peer_ids),
    post_lst(ctx)
  {
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
    
    GtkWidget *tabs(gtk_notebook_new());
    gtk_widget_set_vexpand(tabs, true);
    g_signal_connect(tabs, "switch-page", G_CALLBACK(on_page), this);
    gtk_container_add(GTK_CONTAINER(fields), tabs);

    gtk_notebook_append_page(GTK_NOTEBOOK(tabs),
			     init_general(*this),
			     gtk_label_new_with_mnemonic("_1 General"));

    auto peers_lbl(gtk_label_new_with_mnemonic("_2 Peers"));
    auto &me(whoami(ctx));
    gtk_widget_set_sensitive(peers_lbl, rec.owner_id == me.id);
    gtk_widget_set_sensitive(peer_lst.ptr(), rec.owner_id == me.id);
    gtk_notebook_append_page(GTK_NOTEBOOK(tabs),
			     peer_lst.ptr(),
			     peers_lbl);

    GtkWidget *l(gtk_label_new_with_mnemonic("_3 Post History"));
    if (!find_feed_id(ctx, rec.id)) { gtk_widget_set_sensitive(l, false); }
    gtk_notebook_append_page(GTK_NOTEBOOK(tabs), post_lst.ptr(), l);

    focused = name_fld;
  }

  bool ProjectView::save() {
    rec.name = gtk_entry_get_text(GTK_ENTRY(name_fld));
    rec.active = gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(active_fld));
    rec.tags = word_set(get_str(GTK_ENTRY(tags_fld)));
    rec.info = get_str(GTK_TEXT_VIEW(info_fld));
    db::upsert(ctx.db.projects, rec);
    return true;
  }
}}
