#include <cassert>
#include "snackis/ctx.hpp"
#include "snackis/gui/gui.hpp"
#include "snackis/gui/feed_view.hpp"
#include "snackis/gui/post_view.hpp"

namespace snackis {
namespace gui {
  enum FeedCol {COL_FEED_PTR=0, COL_FEED_ID, COL_FEED_NAME};
  enum PostCol {COL_POST_PTR=0, COL_POST_BY, COL_POST_BODY};
    
  static bool load_posts(PostView &v) {
    Ctx &ctx(v.ctx);
    gtk_list_store_clear(v.post_store);
    auto feed_rec(get_sel_rec<Feed>(GTK_COMBO_BOX(v.feed_fld)));
    if (!feed_rec) { return false; }
    Feed feed(ctx, *feed_rec);
    
    for (auto rec: last_posts(feed, v.rec.at, 7)) {
      Post post(ctx, *rec);
      Peer peer(get_peer_id(ctx, post.by_id));
      
      GtkTreeIter iter;
      gtk_list_store_append(v.post_store, &iter);
      const str by(fmt("%0\n%1",
		       peer.name.c_str(),
		       fmt(post.at, "%a %b %d, %H:%M").c_str()));
      
      gtk_list_store_set(v.post_store, &iter,
			 COL_POST_PTR, rec,
			 COL_POST_BY, by.c_str(),
			 COL_POST_BODY, post.body.c_str(),
			 -1);
    }
    
    return true;
  }

  static void on_feed_sel(gpointer *_, PostView *v) {
    load_posts(*v);
    auto sel(get_sel_rec<Feed>(GTK_COMBO_BOX(v->feed_fld)) ? true : false);
    gtk_widget_set_sensitive(v->edit_feed_btn, sel);
    refresh(*v);
  }

  static void on_edit_feed(gpointer *_, PostView *v) {
    Feed feed(v->ctx, *get_sel_rec<Feed>(GTK_COMBO_BOX(v->feed_fld)));
    FeedView *fv(new FeedView(feed));
    push_view(*fv);
  }
  
  static void init_feeds(PostView &v) {
    Ctx &ctx(v.ctx);

    for(auto key = ctx.db.feeds_sort.recs.begin();
	key != ctx.db.feeds_sort.recs.end();
	key++) {
      auto &rec(db::get(ctx.db.feeds, *key));
      Feed feed(ctx, rec);
      
      if (feed.id == v.rec.feed_id || feed.active) {
	GtkTreeIter iter;
	gtk_list_store_append(v.feed_store, &iter);
	gtk_list_store_set(v.feed_store, &iter,
			   COL_FEED_PTR, &rec,
			   COL_FEED_ID, to_str(feed.id).c_str(),
			   COL_FEED_NAME, feed.name.c_str(),
			   -1);
      }
    }
    
    if (!gtk_combo_box_set_active_id(GTK_COMBO_BOX(v.feed_fld),
				     to_str(v.rec.feed_id).c_str())) {
      gtk_combo_box_set_active(GTK_COMBO_BOX(v.feed_fld), 0);
    }
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
    auto rend(gtk_cell_renderer_text_new());
    auto by_col(gtk_tree_view_column_new_with_attributes("Feed History",
							 rend,
							 "text", COL_POST_BY,
							 nullptr));
    gtk_tree_view_append_column(GTK_TREE_VIEW(v.post_lst), by_col);    

    rend = gtk_cell_renderer_text_new();
    auto body_col(gtk_tree_view_column_new_with_attributes("",
							   rend,
							   "text", COL_POST_BODY,
							   nullptr));
    gtk_tree_view_column_set_expand(GTK_TREE_VIEW_COLUMN(body_col), true);
    gtk_tree_view_append_column(GTK_TREE_VIEW(v.post_lst), body_col);
    g_signal_connect(v.post_lst, "row-activated", G_CALLBACK(on_edit_post), &v);
  }
  
  PostView::PostView(const Post &post):
    RecView("Post", post),
    feed_store(gtk_list_store_new(3, G_TYPE_POINTER, G_TYPE_STRING, G_TYPE_STRING)),
    post_store(gtk_list_store_new(3, G_TYPE_POINTER, G_TYPE_STRING, G_TYPE_STRING)),
    feed_fld(new_combo_box(GTK_TREE_MODEL(feed_store))),
    edit_feed_btn(gtk_button_new_with_mnemonic("_Edit Feed")),
    body_fld(new_text_view()),
    post_lst(gtk_tree_view_new_with_model(GTK_TREE_MODEL(post_store))) {
    GtkWidget *lbl;

    init_feeds(*this);
    lbl = gtk_label_new("Feed");
    gtk_widget_set_halign(lbl, GTK_ALIGN_START);
    gtk_container_add(GTK_CONTAINER(fields), lbl);
    GtkWidget *feed_box = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 5);
    gtk_container_add(GTK_CONTAINER(fields), feed_box);
    gtk_widget_set_hexpand(feed_fld, true);
    g_signal_connect(feed_fld, "changed", G_CALLBACK(on_feed_sel), this);
    gtk_container_add(GTK_CONTAINER(feed_box), feed_fld);
    g_signal_connect(edit_feed_btn, "clicked", G_CALLBACK(on_edit_feed), this);
    gtk_container_add(GTK_CONTAINER(feed_box), edit_feed_btn);
    
    lbl = gtk_label_new("Body");
    gtk_widget_set_halign(lbl, GTK_ALIGN_START);
    gtk_widget_set_margin_top(lbl, 5);
    gtk_container_add(GTK_CONTAINER(fields), lbl);
    gtk_container_add(GTK_CONTAINER(fields), gtk_widget_get_parent(body_fld));
    set_str(GTK_TEXT_VIEW(body_fld), post.body);
    
    init_posts(*this);
    gtk_widget_set_margin_top(post_lst, 5);
    gtk_container_add(GTK_CONTAINER(fields), post_lst);
    lbl = gtk_label_new("Press Return or double-click to edit post");
    gtk_container_add(GTK_CONTAINER(fields), lbl);

    focused = feed_fld;
    on_feed_sel(nullptr, this);
  }

  bool PostView::allow_save() const {
    return
      rec.by_id == whoami(ctx).id &&
      get_sel_rec<Feed>(GTK_COMBO_BOX(feed_fld));
  }

  bool PostView::save() {
    Feed feed(ctx, *get_sel_rec<Feed>(GTK_COMBO_BOX(feed_fld)));
    rec.feed_id = feed.id;
    rec.body = get_str(GTK_TEXT_VIEW(body_fld));
    db::upsert(ctx.db.posts, rec);
    send(rec);
    return true;
  }
}}
