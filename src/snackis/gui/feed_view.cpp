#include <cassert>
#include "snackis/ctx.hpp"
#include "snackis/gui/gui.hpp"
#include "snackis/gui/feed_view.hpp"
#include "snackis/gui/peer_search.hpp"
#include "snackis/gui/post_search.hpp"
#include "snackis/gui/post_view.hpp"
#include "snackis/gui/project_view.hpp"
#include "snackis/gui/task_view.hpp"

namespace snackis {
namespace gui {
  static void on_page(GtkNotebook *w, GtkWidget *p, guint pn, FeedView *v) {
    switch (pn) {
    case 1:
      load(v->peer_lst);
      break;
    case 2:
      load(v->post_lst, v->rec, now());      
      break;
    }
  }

  static void on_project(gpointer *_, FeedView *v) {
    push_view(new ProjectView(get_project_id(v->ctx, v->rec.id)));
  }

  static void on_task(gpointer *_, FeedView *v) {
    push_view(new TaskView(get_task_id(v->ctx, v->rec.id)));
  }

  static void on_post(gpointer *_, FeedView *v) {
    push_view(new PostView(get_post_id(v->ctx, v->rec.id)));
  }

  static void on_find_posts(gpointer *_, FeedView *v) {
    PostSearch *ps = new PostSearch(v->ctx);
    select<Feed>(ps->feed_fld, v->rec);
    push_view(ps);
  }

  static void on_new_post(gpointer *_, FeedView *v) {
    db::Trans trans(v->ctx);
    TRY(try_save);
    v->save();

    if (try_save.errors.empty()) {
      db::commit(trans, fmt("Saved Feed ", id_str(v->rec)));
      Post post(v->ctx);
      set_feed(post, v->rec);
      push_view(new PostView(post));
    }
  }
  
  static GtkWidget *init_general(FeedView &v) {
    auto me(whoami(v.ctx));
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
    set_str(GTK_ENTRY(v.tags_fld), join(v.rec.tags.begin(), v.rec.tags.end(), ' '));

    GtkWidget *lbl = new_label("Info");
    gtk_widget_set_margin_top(lbl, 5);
    gtk_container_add(GTK_CONTAINER(frm), lbl);
    gtk_text_view_set_editable(GTK_TEXT_VIEW(v.info_fld), v.rec.owner_id == me.id);
    gtk_container_add(GTK_CONTAINER(frm), gtk_widget_get_parent(v.info_fld));
    set_str(GTK_TEXT_VIEW(v.info_fld), v.rec.info);

    return frm;
  }

  FeedView::FeedView(const Feed &feed):
    SharedView<Feed>("Feed", feed),
    find_posts_btn(gtk_button_new_with_mnemonic("_Find Posts")),
    new_post_btn(gtk_button_new_with_mnemonic("New _Post")),
    project_btn(gtk_button_new_with_mnemonic("View Project")),
    task_btn(gtk_button_new_with_mnemonic("View Task")),
    post_btn(gtk_button_new_with_mnemonic("View Post")),
    name_fld(gtk_entry_new()),
    tags_fld(gtk_entry_new()),
    active_fld(gtk_check_button_new_with_label("Active")),
    info_fld(new_text_view()),
    peer_lst(ctx, "Peer", this->rec.peer_ids),
    post_lst(ctx)    
  {
    g_signal_connect(find_posts_btn, "clicked", G_CALLBACK(on_find_posts), this);
    gtk_container_add(GTK_CONTAINER(menu), find_posts_btn);
    g_signal_connect(new_post_btn, "clicked", G_CALLBACK(on_new_post), this);
    gtk_container_add(GTK_CONTAINER(menu), new_post_btn);

    if (find_project_id(ctx, rec.id)) {
      g_signal_connect(project_btn, "clicked", G_CALLBACK(on_project), this);
      gtk_container_add(GTK_CONTAINER(menu), project_btn);
    } else if (find_task_id(ctx, rec.id)) {
      g_signal_connect(task_btn, "clicked", G_CALLBACK(on_task), this);
      gtk_container_add(GTK_CONTAINER(menu), task_btn);
    } else if (find_post_id(ctx, rec.id)) {
      g_signal_connect(post_btn, "clicked", G_CALLBACK(on_post), this);
      gtk_container_add(GTK_CONTAINER(menu), post_btn);
    }
    
    GtkWidget *tabs(gtk_notebook_new());
    gtk_widget_set_vexpand(tabs, true);
    g_signal_connect(tabs, "switch-page", G_CALLBACK(on_page), this);
    gtk_container_add(GTK_CONTAINER(fields), tabs);

    gtk_notebook_append_page(GTK_NOTEBOOK(tabs),
			     init_general(*this),
			     gtk_label_new_with_mnemonic("_1 General"));

    gtk_notebook_append_page(GTK_NOTEBOOK(tabs),
			     peer_lst.ptr(),
			     gtk_label_new_with_mnemonic("_2 Peers"));

    gtk_notebook_append_page(GTK_NOTEBOOK(tabs),
			     post_lst.ptr(),
			     gtk_label_new_with_mnemonic("_3 Post History"));
    
    focused = name_fld;
    refresh(*this);
  }

  bool FeedView::save() {
    rec.name = gtk_entry_get_text(GTK_ENTRY(name_fld));
    rec.tags = word_set(get_str(GTK_ENTRY(tags_fld)));
    rec.info = get_str(GTK_TEXT_VIEW(info_fld));
    rec.active = gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(active_fld));
    db::upsert(ctx.db.feeds, rec);
    return true;
  }
}}
