#include <cassert>
#include "snackis/ctx.hpp"
#include "snackis/gui/gui.hpp"
#include "snackis/gui/feed_view.hpp"
#include "snackis/gui/post_search.hpp"
#include "snackis/gui/post_view.hpp"
#include "snackis/gui/project_view.hpp"
#include "snackis/gui/task_view.hpp"

namespace snackis {
namespace gui {
  static void on_page(GtkNotebook *w, GtkWidget *p, guint pn, PostView *v) {
    switch (pn) {
    case 1:
      load(v->peer_lst);
      break;
    }
  }

  static void on_project(gpointer *_, PostView *v) {
    push_view(new ProjectView(get_project_id(v->ctx, v->rec.feed_id)));
  }

  static void on_task(gpointer *_, PostView *v) {
    push_view(new TaskView(get_task_id(v->ctx, v->rec.feed_id)));
  }

  static void on_feed(gpointer *_, PostView *v) {
    push_view(new FeedView(*v->feed_fld.selected));
  }

  static bool save_rec(PostView &v) {
    Ctx &ctx(v.ctx);
    db::Trans trans(ctx);
    TRY(try_save);
    if (!v.save() || !try_save.errors.empty()) { return false; }
    db::commit(trans, fmt("Saved Post %0", id_str(v.rec)));
    return true;
  }
  
  static void on_post(gpointer *_, PostView *v) {
    Post post(v->ctx);
    post.feed_id = v->rec.feed_id;
    push_view(new PostView(post));
  }

  static void on_find_replies(gpointer *_, PostView *v) {
    PostSearch *ps = new PostSearch(v->ctx);
    select<Feed>(ps->feed_fld, get_reply_feed(v->rec));
    push_view(ps);
  }

  static void on_reply(gpointer *_, PostView *v) {
    if (save_rec(*v)) {
      Post post(v->ctx);
      post.feed_id = get_reply_feed(v->rec).id;
      PostView *pv = new PostView(post);
      pv->on_save.push_back([v]() { v->save(); });
      push_view(pv);
    }
  }

  GtkWidget *init_general(PostView &v) {
    Ctx &ctx(v.ctx);
    auto me(whoamid(ctx));
    GtkWidget *frm = gtk_box_new(GTK_ORIENTATION_VERTICAL, 5);
    
    gtk_container_add(GTK_CONTAINER(frm), new_label("Feed"));
    GtkWidget *feed_box = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 5);
    gtk_container_add(GTK_CONTAINER(frm), feed_box);

    v.feed_fld.on_change.emplace([&v]() {
	clear(v.post_lst);

	if (v.feed_fld.selected) {
	  set_feed(v.rec, *v.feed_fld.selected);
	  auto reply_fd(find_feed_id(v.ctx, v.rec.id));
	  gui::load(v.post_lst, *v.feed_fld.selected, v.rec.created_at);
	  load(v.peer_lst);
	  set_str(GTK_ENTRY(v.tags_fld),
		  join(v.rec.tags.begin(), v.rec.tags.end(), ' '));
	  gtk_widget_set_sensitive(v.feed_btn, true);
	} else {
	  gtk_widget_set_sensitive(v.feed_btn, false);
	}
	
	refresh(v);
      });

    gtk_widget_set_sensitive(v.feed_fld.ptr(), v.rec.owner_id == me);    
    gtk_container_add(GTK_CONTAINER(feed_box), v.feed_fld.ptr());
    g_signal_connect(v.feed_btn, "clicked", G_CALLBACK(on_feed), &v);
    gtk_widget_set_sensitive(v.feed_btn, false);
    gtk_container_add(GTK_CONTAINER(feed_box), v.feed_btn);

    if (v.rec.feed_id != null_uid) {
      select<Feed>(v.feed_fld, get_feed_id(ctx, v.rec.feed_id));
    }

    GtkWidget *lbl(new_label("Tags"));
    gtk_widget_set_margin_top(lbl, 5);
    gtk_container_add(GTK_CONTAINER(frm), lbl);
    gtk_container_add(GTK_CONTAINER(frm), v.tags_fld);
    set_str(GTK_ENTRY(v.tags_fld), join(v.rec.tags.begin(), v.rec.tags.end(), ' '));

    gtk_container_add(GTK_CONTAINER(frm), new_label("Body"));
    gtk_text_view_set_editable(GTK_TEXT_VIEW(v.body_fld), v.rec.owner_id == me);
    gtk_container_add(GTK_CONTAINER(frm), gtk_widget_get_parent(v.body_fld));
    set_str(GTK_TEXT_VIEW(v.body_fld), v.rec.body);

    gtk_widget_set_margin_top(v.post_lst.ptr(), 5);
    gtk_container_add(GTK_CONTAINER(frm), v.post_lst.ptr());
    return frm;
  }
  
  PostView::PostView(const Post &post):
    SharedView<Post>("Post", post),
    post_btn(gtk_button_new_with_mnemonic("New _Post")),
    find_replies_btn(gtk_button_new_with_mnemonic("_Find Replies")),
    reply_btn(gtk_button_new_with_mnemonic("New _Reply")),
    project_btn(gtk_button_new_with_mnemonic("View Project")),
    task_btn(gtk_button_new_with_mnemonic("View Task")),
    feed_btn(gtk_button_new_with_mnemonic("_View Feed")),
    tags_fld(gtk_entry_new()),
    body_fld(new_text_view()),
    feed_fld(ctx),
    peer_lst(ctx, "Peer", this->rec.peer_ids),
    post_lst(ctx)
  {
    g_signal_connect(post_btn, "clicked", G_CALLBACK(on_post), this);
    gtk_container_add(GTK_CONTAINER(menu), post_btn);
    gtk_widget_set_sensitive(find_replies_btn,
			     find_feed_id(ctx, rec.id) ? true : false);
    g_signal_connect(find_replies_btn, "clicked", G_CALLBACK(on_find_replies), this);
    gtk_container_add(GTK_CONTAINER(menu), find_replies_btn);
    g_signal_connect(reply_btn, "clicked", G_CALLBACK(on_reply), this);
    gtk_container_add(GTK_CONTAINER(menu), reply_btn);

    if (find_project_id(ctx, rec.feed_id)) {
      g_signal_connect(project_btn, "clicked", G_CALLBACK(on_project), this);
      gtk_container_add(GTK_CONTAINER(menu), project_btn);
    } else if (find_task_id(ctx, rec.feed_id)) {
      g_signal_connect(task_btn, "clicked", G_CALLBACK(on_task), this);
      gtk_container_add(GTK_CONTAINER(menu), task_btn);
    }

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

    focused = (rec.owner_id == me)
      ? feed_fld.search_btn
      : tags_fld;
    refresh(*this);
  }

  bool PostView::allow_save() const {
    return feed_fld.selected ? true : false;
  }

  bool PostView::save() {
    rec.tags = word_set(get_str(GTK_ENTRY(tags_fld)));
    rec.body = get_str(GTK_TEXT_VIEW(body_fld));
    db::upsert(ctx.db.posts, rec);
    return true;
  }
}}
