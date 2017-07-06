#include <cassert>
#include "snackis/ctx.hpp"
#include "snackis/gui/gui.hpp"
#include "snackis/gui/feed_view.hpp"
#include "snackis/gui/post_search.hpp"
#include "snackis/gui/post_view.hpp"
#include "snackis/gui/queue_view.hpp"
#include "snackis/gui/task_search.hpp"
#include "snackis/gui/task_view.hpp"

namespace snackis {
namespace gui {
  static void on_find_tasks(gpointer *_, QueueView *v) {
    TaskSearch *ts = new TaskSearch(v->ctx);
    select<Queue>(ts->queue_fld, v->rec);
    push_view(*ts);
  }

  static void on_new_task(gpointer *_, QueueView *v) {
    Task tsk(v->ctx);
    tsk.queue_ids.insert(v->rec.id);
    TaskView *tv = new TaskView(tsk);
    push_view(*tv);
  }

  static void on_find_posts(gpointer *_, TaskView *v) {
    PostSearch *ps = new PostSearch(v->ctx);
    select<Feed>(ps->feed_fld, get_feed(v->rec));
    push_view(*ps);
  }

  static void on_post(gpointer *_, QueueView *v) {
    db::Trans trans(v->ctx);
    TRY(try_save);
    v->save();

    if (try_save.errors.empty()) {
      db::commit(trans);
      Post post(v->ctx);
      post.feed_id = get_feed(v->rec).id;
      PostView *pv = new PostView(post);
      push_view(*pv);
    }
  }

  QueueView::QueueView(const Queue &rec):
    RecView<Queue>("Queue", rec),
    find_tasks_btn(gtk_button_new_with_mnemonic("_Find Tasks")),
    new_task_btn(gtk_button_new_with_mnemonic("New _Task")),
    find_posts_btn(gtk_button_new_with_mnemonic("Find Posts")),
    post_btn(gtk_button_new_with_mnemonic("New _Post")),
    name_fld(gtk_entry_new()),
    info_fld(new_text_view()),
    peer_lst(ctx, "Peer", this->rec.peer_ids)
  {
    GtkWidget *lbl;

    GtkWidget *btns = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 5);
    gtk_widget_set_margin_bottom(btns, 5);
    gtk_container_add(GTK_CONTAINER(fields), btns);
    g_signal_connect(find_tasks_btn, "clicked", G_CALLBACK(on_find_tasks), this);
    gtk_container_add(GTK_CONTAINER(btns), find_tasks_btn);
    g_signal_connect(new_task_btn, "clicked", G_CALLBACK(on_new_task), this);
    gtk_container_add(GTK_CONTAINER(btns), new_task_btn);
    g_signal_connect(find_posts_btn, "clicked", G_CALLBACK(on_find_posts), this);
    gtk_container_add(GTK_CONTAINER(btns), find_posts_btn);
    g_signal_connect(post_btn, "clicked", G_CALLBACK(on_post), this);
    gtk_container_add(GTK_CONTAINER(btns), post_btn);
    
    lbl = gtk_label_new("Name");
    gtk_widget_set_halign(lbl, GTK_ALIGN_START);
    gtk_container_add(GTK_CONTAINER(fields), lbl);
    gtk_widget_set_hexpand(name_fld, true);    
    gtk_container_add(GTK_CONTAINER(fields), name_fld);
    gtk_entry_set_text(GTK_ENTRY(name_fld), rec.name.c_str());
    
    lbl = gtk_label_new("Info");
    gtk_widget_set_halign(lbl, GTK_ALIGN_START);
    gtk_container_add(GTK_CONTAINER(fields), lbl);
    gtk_container_add(GTK_CONTAINER(fields), gtk_widget_get_parent(info_fld));
    set_str(GTK_TEXT_VIEW(info_fld), rec.info);
    
    gtk_widget_set_margin_top(peer_lst.ptr(), 5);
    gtk_container_add(GTK_CONTAINER(fields), peer_lst.ptr());

    load(peer_lst);
    focused = name_fld;
  }

  bool QueueView::allow_save() const {
    return rec.owner_id == whoami(ctx).id;
  }
  
  bool QueueView::save() {
    rec.name = gtk_entry_get_text(GTK_ENTRY(name_fld));
    rec.info = get_str(GTK_TEXT_VIEW(info_fld));
    db::upsert(ctx.db.queues, rec);
    send(rec);
    return true;
  }
}}
