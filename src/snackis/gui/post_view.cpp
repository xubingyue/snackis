#include <cassert>
#include "snackis/ctx.hpp"
#include "snackis/gui/gui.hpp"
#include "snackis/gui/post_view.hpp"

namespace snackis {
namespace gui {
  enum FeedCol {COL_FEED_PTR=0, COL_FEED_NAME};
  enum PostCol {COL_POST_PTR=0, COL_POST_BY, COL_POST_BODY};
    
  static void on_cancel(gpointer *_, PostView *v) {
    log(v->ctx, "Cancelled post");
    pop_view(*v);
  }

  static bool load_posts(PostView &v) {
    Ctx &ctx(v.ctx);
    gtk_list_store_clear(v.posts);
    auto feed_rec(get_sel_rec<Feed>(GTK_COMBO_BOX(v.feed)));
    if (!feed_rec) { return false; }
    Feed feed(ctx, *feed_rec);
    
    for (auto rec: last_posts(feed, v.post.at, 7)) {
      Post post(ctx, *rec);
      Peer peer(get_peer_id(ctx, post.by_id));
      
      GtkTreeIter iter;
      gtk_list_store_append(v.posts, &iter);
      const str by(fmt("%0\n%1",
		       peer.name.c_str(),
		       fmt(post.at, "%a %b %d, %H:%M:%S").c_str()));
      
      gtk_list_store_set(v.posts, &iter,
			 COL_POST_PTR, rec,
			 COL_POST_BY, by.c_str(),
			 COL_POST_BODY, post.body.c_str(),
			 -1);
    }
    
    return true;
  }

  static void on_feed_sel(gpointer *_, PostView *v) {
    load_posts(*v);
  }

  static void on_save(gpointer *_, PostView *v) {
    Ctx &ctx(v->ctx);
    db::Trans trans(ctx);
    
    Feed feed(ctx, *get_sel_rec<Feed>(GTK_COMBO_BOX(v->feed)));
    activate(feed);
    v->post.feed_id = feed.id;
    v->post.body = get_str(GTK_TEXT_VIEW(v->body_text));
    db::insert(ctx.db.posts, v->post);
    send(v->post);
    db::commit(trans);
    log(v->ctx, "Saved post");
    pop_view(*v);
  }

  void init_feeds(PostView &v) {
    Ctx &ctx(v.ctx);
    size_t cnt(0);
    
    for(const auto &f: ctx.db.feeds.recs) {
      if (*db::get(f, ctx.db.feed_id) == v.post.feed_id ||
	  *db::get(f, ctx.db.feed_active)) {
	GtkTreeIter iter;
	gtk_list_store_append(v.feeds, &iter);
	gtk_list_store_set(v.feeds, &iter,
			   COL_FEED_PTR, &f,
			   COL_FEED_NAME, db::get(f, ctx.db.feed_name)->c_str(),
			   -1);
	cnt++;
      }
    }
    
    auto col(gtk_cell_renderer_text_new());
    gtk_cell_layout_pack_start(GTK_CELL_LAYOUT(v.feed), col, true);
    gtk_cell_layout_set_attributes(GTK_CELL_LAYOUT(v.feed),
				   col,
                                   "text", COL_FEED_NAME,
				   nullptr);
    gtk_combo_box_set_active(GTK_COMBO_BOX(v.feed), 0);
    gtk_widget_set_sensitive(v.save, cnt > 0);
  }

  static db::Rec<Post> *get_sel_post(PostView &v) {
    GtkTreeSelection *sel = gtk_tree_view_get_selection(GTK_TREE_VIEW(v.post_list));
    GtkTreeIter iter;

    if (!gtk_tree_selection_get_selected(sel, nullptr, &iter)) { return nullptr; }
    db::Rec<Post> *rec(nullptr);
    gtk_tree_model_get(GTK_TREE_MODEL(v.posts), &iter, COL_POST_PTR, &rec, -1);
    return rec;
  }

  static void on_edit_post(GtkTreeView *treeview,
			   GtkTreePath *path,
			   GtkTreeViewColumn *col,
			   PostView *v) {
    Ctx &ctx(v->ctx);
    auto post_rec(get_sel_post(*v));
    assert(post_rec);
    Post post(ctx, *post_rec);

    if (post.by_id == whoami(ctx).id) {
      PostView *pv(new PostView(post));
      push_view(*pv);
    }
  }
  
  static void init_posts(PostView &v) {
    gtk_widget_set_hexpand(v.post_list, true);
    gtk_widget_set_vexpand(v.post_list, true);
    auto rend(gtk_cell_renderer_text_new());
    auto by_col(gtk_tree_view_column_new_with_attributes("Feed History",
							 rend,
							 "text", COL_POST_BY,
							 nullptr));
    gtk_tree_view_append_column(GTK_TREE_VIEW(v.post_list), by_col);    

    rend = gtk_cell_renderer_text_new();
    auto body_col(gtk_tree_view_column_new_with_attributes("",
							   rend,
							   "text", COL_POST_BODY,
							   nullptr));
    gtk_tree_view_column_set_expand(GTK_TREE_VIEW_COLUMN(body_col), true);
    gtk_tree_view_append_column(GTK_TREE_VIEW(v.post_list), body_col);
    g_signal_connect(v.post_list, "row-activated", G_CALLBACK(on_edit_post), &v);
    load_posts(v);
  }
  
  PostView::PostView(const Post &post):
    View(post.ctx, "Post"),
    post(post),
    feeds(gtk_list_store_new(2, G_TYPE_POINTER, G_TYPE_STRING)),
    posts(gtk_list_store_new(3,
			     G_TYPE_POINTER,
			     G_TYPE_STRING, G_TYPE_STRING)),
    feed(gtk_combo_box_new_with_model(GTK_TREE_MODEL(feeds))),
    body_text(gtk_text_view_new()),
    body(gtk_scrolled_window_new(NULL, NULL)),
    post_list(gtk_tree_view_new_with_model(GTK_TREE_MODEL(posts))),
    save(gtk_button_new_with_mnemonic("_Save Post")),
    cancel(gtk_button_new_with_mnemonic("_Cancel")) {
    GtkWidget *lbl;

    GtkWidget *frm = gtk_box_new(GTK_ORIENTATION_VERTICAL, 5);
    gtk_box_pack_start(GTK_BOX(panel), frm, true, true, 0);
    
    init_feeds(*this);
    lbl = gtk_label_new("Feed");
    gtk_widget_set_halign(lbl, GTK_ALIGN_START);
    gtk_container_add(GTK_CONTAINER(frm), lbl);
    gtk_widget_set_hexpand(feed, true);
    g_signal_connect(feed, "changed", G_CALLBACK(on_feed_sel), this);
    gtk_container_add(GTK_CONTAINER(frm), feed);
    
    lbl = gtk_label_new("Body");
    gtk_widget_set_halign(lbl, GTK_ALIGN_START);
    gtk_widget_set_margin_top(lbl, 5);
    gtk_container_add(GTK_CONTAINER(frm), lbl);
    gtk_widget_set_hexpand(body_text, true);
    gtk_widget_set_vexpand(body_text, true);
    gtk_text_view_set_wrap_mode(GTK_TEXT_VIEW(body_text), GTK_WRAP_WORD);
    gtk_scrolled_window_set_overlay_scrolling(GTK_SCROLLED_WINDOW(body),
					      false);
    gtk_scrolled_window_set_policy(GTK_SCROLLED_WINDOW(body),
				   GTK_POLICY_NEVER,
				   GTK_POLICY_ALWAYS);
    gtk_container_add(GTK_CONTAINER(body), body_text);
    gtk_container_add(GTK_CONTAINER(frm), body);
    set_str(GTK_TEXT_VIEW(body_text), post.body);
    
    init_posts(*this);
    gtk_widget_set_margin_top(post_list, 5);
    gtk_container_add(GTK_CONTAINER(frm), post_list);
    lbl = gtk_label_new("Press Return or double-click to edit selected post");
    gtk_container_add(GTK_CONTAINER(frm), lbl);

    GtkWidget *btns = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 5);
    gtk_widget_set_halign(btns, GTK_ALIGN_END);
    gtk_widget_set_valign(btns, GTK_ALIGN_END);
    gtk_widget_set_margin_top(btns, 10);
    gtk_container_add(GTK_CONTAINER(panel), btns);
        
    g_signal_connect(save, "clicked", G_CALLBACK(on_save), this);
    gtk_container_add(GTK_CONTAINER(btns), save);

    g_signal_connect(cancel, "clicked", G_CALLBACK(on_cancel), this);
    gtk_container_add(GTK_CONTAINER(btns), cancel);
    focused = feed;
  }
}}
