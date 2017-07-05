#include <cassert>
#include "snackis/ctx.hpp"
#include "snackis/gui/gui.hpp"
#include "snackis/gui/feed_view.hpp"
#include "snackis/gui/post_view.hpp"

namespace snackis {
namespace gui {
  enum PostCol {COL_POST_PTR=0, COL_POST_BY, COL_POST_BODY};
    
  static bool load_posts(PostView &v) {
    Ctx &ctx(v.ctx);
    gtk_list_store_clear(v.post_store);

    auto feed(v.feed_fld.selected);
    if (!feed) { return false; }
    
    for (auto rec: last_posts(*feed, v.rec.created_at, 30)) {
      Post post(ctx, *rec);
      Peer peer(get_peer_id(ctx, post.owner_id));
      
      GtkTreeIter iter;
      gtk_list_store_append(v.post_store, &iter);
      const str by(fmt("%0\n%1",
		       peer.name.c_str(),
		       fmt(post.created_at, "%a %b %d, %H:%M").c_str()));
      
      gtk_list_store_set(v.post_store, &iter,
			 COL_POST_PTR, rec,
			 COL_POST_BY, by.c_str(),
			 COL_POST_BODY, post.body.c_str(),
			 -1);
    }
    
    return true;
  }

  static void on_edit_feed(gpointer *_, PostView *v) {
    FeedView *fv(new FeedView(*v->feed_fld.selected));
    push_view(*fv);
  }
  
  static void on_edit_post(GtkTreeView *treeview,
			   GtkTreePath *path,
			   GtkTreeViewColumn *col,
			   PostView *v) {
    Ctx &ctx(v->ctx);
    auto post_rec(get_sel_rec<Post>(GTK_TREE_VIEW(v->post_lst)));
    assert(post_rec);
    Post post(ctx, *post_rec);
    PostView *pv(new PostView(post));
    push_view(*pv);
  }
  
  static void init_posts(PostView &v) {
    gtk_widget_set_hexpand(v.post_lst, true);
    gtk_widget_set_vexpand(v.post_lst, true);
    add_col(GTK_TREE_VIEW(v.post_lst), "Feed History", COL_POST_BY);
    add_col(GTK_TREE_VIEW(v.post_lst), "", COL_POST_BODY, true);
    g_signal_connect(v.post_lst, "row-activated", G_CALLBACK(on_edit_post), &v);
  }

  PostView::PostView(const Post &post):
    RecView("Post", post),
    post_store(gtk_list_store_new(3, G_TYPE_POINTER, G_TYPE_STRING, G_TYPE_STRING)),
    edit_feed_btn(gtk_button_new_with_mnemonic("_Edit Feed")),
    body_fld(new_text_view()),
    post_lst(new_tree_view(GTK_TREE_MODEL(post_store))),
    feed_fld(ctx)
  {
    GtkWidget *lbl;

    lbl = gtk_label_new("Feed");
    gtk_widget_set_halign(lbl, GTK_ALIGN_START);
    gtk_container_add(GTK_CONTAINER(fields), lbl);
    GtkWidget *feed_box = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 5);
    gtk_container_add(GTK_CONTAINER(fields), feed_box);

    feed_fld.on_change.emplace([this]() {
	load_posts(*this);
	auto sel(feed_fld.selected ? true : false);
	gtk_widget_set_sensitive(edit_feed_btn, sel);
	refresh(*this);
      });

    gtk_container_add(GTK_CONTAINER(feed_box), feed_fld.ptr());
    g_signal_connect(edit_feed_btn, "clicked", G_CALLBACK(on_edit_feed), this);
    gtk_widget_set_sensitive(edit_feed_btn, false);
    gtk_container_add(GTK_CONTAINER(feed_box), edit_feed_btn);
    
    lbl = gtk_label_new("Body");
    gtk_widget_set_halign(lbl, GTK_ALIGN_START);
    gtk_widget_set_margin_top(lbl, 5);
    gtk_container_add(GTK_CONTAINER(fields), lbl);
    gtk_container_add(GTK_CONTAINER(fields), gtk_widget_get_parent(body_fld));
    set_str(GTK_TEXT_VIEW(body_fld), rec.body);
    
    init_posts(*this);
    gtk_widget_set_margin_top(post_lst, 10);
    gtk_container_add(GTK_CONTAINER(fields), gtk_widget_get_parent(post_lst));
    lbl = gtk_label_new("Press Return or double-click to edit Post");
    gtk_container_add(GTK_CONTAINER(fields), lbl);

    if (rec.feed_id == null_uid) {
      refresh(*this);
    } else {
      select<Feed>(feed_fld, get_feed_id(ctx, rec.feed_id));
    }

    focused = feed_fld.search_btn;
  }

  bool PostView::allow_save() const {
    return rec.owner_id == whoami(ctx).id && feed_fld.selected;
  }

  bool PostView::save() {
    set_feed(rec, *feed_fld.selected);
    rec.body = get_str(GTK_TEXT_VIEW(body_fld));
    db::upsert(ctx.db.posts, rec);
    send(rec);
    return true;
  }
}}
