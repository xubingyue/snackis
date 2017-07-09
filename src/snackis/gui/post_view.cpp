#include <cassert>
#include "snackis/ctx.hpp"
#include "snackis/gui/gui.hpp"
#include "snackis/gui/feed_view.hpp"
#include "snackis/gui/post_search.hpp"
#include "snackis/gui/post_view.hpp"

namespace snackis {
namespace gui {
  static void on_edit_feed(gpointer *_, PostView *v) {
    FeedView *fv(new FeedView(*v->feed_fld.selected));
    push_view(*fv);
  }
  
  static void on_post(gpointer *_, PostView *v) {
    Post post(v->ctx);
    post.feed_id = v->rec.feed_id;
    PostView *pv = new PostView(post);
    push_view(*pv);
  }

  static void on_find_replies(gpointer *_, PostView *v) {
    PostSearch *ps = new PostSearch(v->ctx);
    select<Feed>(ps->feed_fld, get_reply_feed(v->rec));
    push_view(*ps);
  }

  static void on_reply(gpointer *_, PostView *v) {
    Post post(v->ctx);
    post.feed_id = get_reply_feed(v->rec).id;
    PostView *pv = new PostView(post);
    pv->on_save.push_back([v]() { v->save(); });
    push_view(*pv);
  }
  
  PostView::PostView(const Post &post):
    SharedView<Post>("Post", post),
    post_btn(gtk_button_new_with_mnemonic("New _Post")),
    find_replies_btn(gtk_button_new_with_mnemonic("_Find Replies")),
    reply_btn(gtk_button_new_with_mnemonic("New _Reply")),
    edit_feed_btn(gtk_button_new_with_mnemonic("_Edit Feed")),
    tags_fld(gtk_entry_new()),
    body_fld(new_text_view()),
    feed_fld(ctx),
    post_lst(ctx, "History")
  {
    GtkWidget *lbl;

    g_signal_connect(post_btn, "clicked", G_CALLBACK(on_post), this);
    gtk_container_add(GTK_CONTAINER(menu), post_btn);
    gtk_widget_set_sensitive(find_replies_btn,
			     find_feed_id(ctx, rec.id) ? true : false);
    g_signal_connect(find_replies_btn, "clicked", G_CALLBACK(on_find_replies), this);
    gtk_container_add(GTK_CONTAINER(menu), find_replies_btn);
    g_signal_connect(reply_btn, "clicked", G_CALLBACK(on_reply), this);
    gtk_container_add(GTK_CONTAINER(menu), reply_btn);

    lbl = gtk_label_new("Feed");
    gtk_widget_set_halign(lbl, GTK_ALIGN_START);
    gtk_container_add(GTK_CONTAINER(fields), lbl);
    GtkWidget *feed_box = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 5);
    gtk_container_add(GTK_CONTAINER(fields), feed_box);

    feed_fld.on_change.emplace([this]() {
	clear(post_lst);

	if (feed_fld.selected) {
	  set_feed(rec, *feed_fld.selected);
	  auto reply_fd(find_feed_id(ctx, rec.id));
	  load(post_lst, *feed_fld.selected, rec.created_at);
	}
	
	set_str(GTK_ENTRY(tags_fld), join(rec.tags.begin(), rec.tags.end(), ' '));
	auto sel(feed_fld.selected ? true : false);
	gtk_widget_set_sensitive(edit_feed_btn, sel);
	refresh(*this);
      });

    gtk_container_add(GTK_CONTAINER(feed_box), feed_fld.ptr());
    g_signal_connect(edit_feed_btn, "clicked", G_CALLBACK(on_edit_feed), this);
    gtk_widget_set_sensitive(edit_feed_btn, false);
    gtk_container_add(GTK_CONTAINER(feed_box), edit_feed_btn);
    
    lbl = gtk_label_new("Tags");
    gtk_widget_set_halign(lbl, GTK_ALIGN_START);
    gtk_widget_set_margin_top(lbl, 5);
    gtk_container_add(GTK_CONTAINER(fields), lbl);
    gtk_container_add(GTK_CONTAINER(fields), tags_fld);
    set_str(GTK_ENTRY(tags_fld), join(rec.tags.begin(), rec.tags.end(), ' '));

    lbl = gtk_label_new("Body");
    gtk_widget_set_halign(lbl, GTK_ALIGN_START);
    gtk_container_add(GTK_CONTAINER(fields), lbl);
    gtk_container_add(GTK_CONTAINER(fields), gtk_widget_get_parent(body_fld));
    set_str(GTK_TEXT_VIEW(body_fld), rec.body);

    gtk_widget_set_margin_top(post_lst.ptr(), 5);
    gtk_container_add(GTK_CONTAINER(fields), post_lst.ptr());

    if (rec.feed_id == null_uid) {
      refresh(*this);
    } else {
      select<Feed>(feed_fld, get_feed_id(ctx, rec.feed_id));
    }

    focused = feed_fld.search_btn;
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
