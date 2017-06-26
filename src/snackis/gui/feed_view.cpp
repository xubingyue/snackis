#include <cassert>
#include "snackis/ctx.hpp"
#include "snackis/gui/gui.hpp"
#include "snackis/gui/feed_view.hpp"
#include "snackis/gui/post_view.hpp"

namespace snackis {
namespace gui {
  enum PeerCol {COL_PEER_PTR=0, COL_PEER_ID, COL_PEER_NAME};
  enum PostCol {COL_POST_PTR=0, COL_POST_BY, COL_POST_BODY};
  
  static void on_peer_sel(gpointer *_, FeedView *v) {
    Ctx &ctx(v->ctx);
    auto rec(get_sel_rec<Peer>(GTK_COMBO_BOX(v->peer_fld)));
    auto id(*db::get(*rec, ctx.db.peer_id));
    bool exists = v->rec.peer_ids.find(id) != v->rec.peer_ids.end();
    gtk_widget_set_sensitive(v->add_peer_btn, !exists);
  }
  
  static void on_add_peer(gpointer *_, FeedView *v) {
    Ctx &ctx(v->ctx);
    auto rec(get_sel_rec<Peer>(GTK_COMBO_BOX(v->peer_fld)));
    CHECK(rec ? true : false, _);
    Peer peer(ctx, *rec);
    
    GtkTreeIter iter;    
    gtk_list_store_append(v->feed_peer_store, &iter);
    gtk_list_store_set(v->feed_peer_store, &iter,
		       COL_PEER_PTR, rec,
		       COL_PEER_ID, to_str(peer.id).c_str(),
		       COL_PEER_NAME, peer.name.c_str(),
		       -1);
    auto sel(gtk_tree_view_get_selection(GTK_TREE_VIEW(v->peer_lst)));
    gtk_tree_selection_select_iter(sel, &iter);
  
    v->rec.peer_ids.insert(peer.id);
    gtk_widget_set_sensitive(v->add_peer_btn, false);
    gtk_widget_grab_focus(v->peer_fld);
  }

  static void on_remove_peer(GtkTreeView *treeview,
			     GtkTreePath *path,
			     GtkTreeViewColumn *col,
			     FeedView *v) {
    Ctx &ctx(v->ctx);
    auto iter(get_sel_iter(GTK_TREE_VIEW(v->peer_lst)));

    if (iter) {
      auto it(*iter);
      auto rec(get_sel_rec<Peer>(GTK_TREE_VIEW(v->peer_lst), it));
      CHECK(rec ? true : false, _);
      v->rec.peer_ids.erase(*db::get(*rec, ctx.db.peer_id));
      gtk_list_store_remove(v->feed_peer_store, &it);
    }

    on_peer_sel(nullptr, v);
  }
  
  static void load_peers(FeedView &v) {
    Ctx &ctx(v.ctx);
    
    for (const auto &peer_id: v.rec.peer_ids) {
      db::Rec<Peer> key;
      db::set(key, ctx.db.peer_id, peer_id);
      const db::Rec<Peer> &rec(db::get(ctx.db.peers, key));
      
      GtkTreeIter iter;
      gtk_list_store_append(v.feed_peer_store, &iter);
      gtk_list_store_set(v.feed_peer_store, &iter,
			 COL_PEER_PTR, &rec,
			 COL_PEER_NAME, db::get(rec, ctx.db.peer_name)->c_str(),
			 -1);
    }
  }
  
  static void init_peers(FeedView &v) {
    Ctx &ctx(v.ctx);

    gtk_widget_set_margin_top(v.peer_lst, 5);
    gtk_widget_set_hexpand(v.peer_lst, true);
    auto rend(gtk_cell_renderer_text_new());
    auto name_col(gtk_tree_view_column_new_with_attributes("Peers",
							   rend,
							   "text", COL_PEER_NAME,
							   nullptr));
    gtk_tree_view_append_column(GTK_TREE_VIEW(v.peer_lst), name_col);    
    g_signal_connect(v.peer_lst, "row-activated", G_CALLBACK(on_remove_peer), &v);
    
    for(auto key = ctx.db.peers_sort.recs.begin();
	key != ctx.db.peers_sort.recs.end();
	key++) {
      auto &rec(db::get(ctx.db.peers, *key));
      Peer peer(ctx, rec);
      
      GtkTreeIter iter;
      gtk_list_store_append(v.peer_store, &iter);
      gtk_list_store_set(v.peer_store, &iter,
			 COL_PEER_PTR, &rec,
			 COL_PEER_NAME, peer.name.c_str(),
			 -1);
    }

    gtk_container_add(GTK_CONTAINER(v.fields), v.peer_lst);
    gtk_container_add(GTK_CONTAINER(v.fields),
		      gtk_label_new("Press Return or double-click to remove peer"));

    GtkWidget *peer_box(gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 5));
    gtk_container_add(GTK_CONTAINER(v.fields), peer_box);

    gtk_widget_set_hexpand(v.peer_fld, true);
    g_signal_connect(v.peer_fld, "changed", G_CALLBACK(on_peer_sel), &v);
    gtk_combo_box_set_active(GTK_COMBO_BOX(v.peer_fld), 0);
    gtk_container_add(GTK_CONTAINER(peer_box), v.peer_fld);

    g_signal_connect(v.add_peer_btn, "clicked", G_CALLBACK(on_add_peer), &v);
    gtk_container_add(GTK_CONTAINER(peer_box), v.add_peer_btn);
    load_peers(v);
  }

  static void on_edit_post(GtkTreeView *treeview,
			   GtkTreePath *path,
			   GtkTreeViewColumn *col,
			   FeedView *v) {
    Ctx &ctx(v->ctx);
    auto post_rec(get_sel_rec<Post>(GTK_TREE_VIEW(v->post_lst)));
    assert(post_rec);
    Post post(ctx, *post_rec);
    PostView *pv(new PostView(post));
    push_view(*pv);
  }
  
  static void load_posts(FeedView &v) {
    Ctx &ctx(v.ctx);

    for (auto rec: last_posts(v.rec, Time::max(), 7)) {
      Post post(ctx, *rec);
      Peer peer(get_peer_id(ctx, post.by_id));
      
      GtkTreeIter iter;
      gtk_list_store_append(v.post_store, &iter);
      const str by(fmt("%0\n%1",
		       peer.name.c_str(),
		       fmt(post.at, "%a %b %d, %H:%M:%S").c_str()));
      
      gtk_list_store_set(v.post_store, &iter,
			 COL_POST_PTR, rec,
			 COL_POST_BY, by.c_str(),
			 COL_POST_BODY, post.body.c_str(),
			 -1);
    }
  }

  static void init_posts(FeedView &v) {
    gtk_widget_set_hexpand(v.post_lst, true);
    gtk_widget_set_vexpand(v.post_lst, true);
    auto rend(gtk_cell_renderer_text_new());
    auto by_col(gtk_tree_view_column_new_with_attributes("Post History",
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
    load_posts(v);
  }
  
  FeedView::FeedView(const Feed &feed):
    RecView("Feed", feed),
    peer_store(gtk_list_store_new(3, G_TYPE_POINTER, G_TYPE_STRING, G_TYPE_STRING)),
    feed_peer_store(gtk_list_store_new(2, G_TYPE_POINTER, G_TYPE_STRING)),
    post_store(gtk_list_store_new(3,
				  G_TYPE_POINTER,
				  G_TYPE_STRING, G_TYPE_STRING)),
    name_fld(gtk_entry_new()),
    active_fld(gtk_check_button_new_with_label("Active")),
    info_fld(new_text_view()),
    peer_lst(gtk_tree_view_new_with_model(GTK_TREE_MODEL(feed_peer_store))),
    peer_fld(new_combo_box(GTK_TREE_MODEL(peer_store))),
    add_peer_btn(gtk_button_new_with_mnemonic("_Add Peer")),
    post_lst(gtk_tree_view_new_with_model(GTK_TREE_MODEL(post_store))),
    post_fld(new_text_view()) {
    GtkWidget *lbl;

    lbl = gtk_label_new("Name");
    gtk_widget_set_halign(lbl, GTK_ALIGN_START);
    gtk_container_add(GTK_CONTAINER(fields), lbl);
    GtkWidget *name_box = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 5);
    gtk_container_add(GTK_CONTAINER(fields), name_box);
    gtk_widget_set_hexpand(name_fld, true);
    gtk_container_add(GTK_CONTAINER(name_box), name_fld);    
    gtk_entry_set_text(GTK_ENTRY(name_fld), feed.name.c_str());
    gtk_container_add(GTK_CONTAINER(name_box), active_fld);
    gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(active_fld), feed.active);

    lbl = gtk_label_new("Info");
    gtk_widget_set_halign(lbl, GTK_ALIGN_START);
    gtk_container_add(GTK_CONTAINER(fields), lbl);
    gtk_container_add(GTK_CONTAINER(fields), gtk_widget_get_parent(info_fld));
    set_str(GTK_TEXT_VIEW(info_fld), feed.info);
    
    init_peers(*this);

    init_posts(*this);
    gtk_widget_set_margin_top(post_lst, 5);
    gtk_container_add(GTK_CONTAINER(fields), post_lst);
    lbl = gtk_label_new("Press Return or double-click to edit post");
    gtk_container_add(GTK_CONTAINER(fields), lbl);

    lbl = gtk_label_new("New Post");
    gtk_widget_set_halign(lbl, GTK_ALIGN_START);
    gtk_widget_set_margin_top(lbl, 5);
    gtk_container_add(GTK_CONTAINER(fields), lbl);
    gtk_container_add(GTK_CONTAINER(fields), gtk_widget_get_parent(post_fld));
    
    focused = name_fld;
  }

  bool FeedView::allow_save() const {
    return rec.owner_id == whoami(ctx).id;
  }

  bool FeedView::save() {
    rec.name = gtk_entry_get_text(GTK_ENTRY(name_fld));
    rec.info = get_str(GTK_TEXT_VIEW(info_fld));
    rec.active = gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(active_fld));
    db::upsert(ctx.db.feeds, rec);
    const str post_body(get_str(GTK_TEXT_VIEW(post_fld)));

    if (!post_body.empty()) {
      Post post(ctx);
      post.feed_id = rec.id;
      post.body = post_body;
      db::insert(ctx.db.posts, post);
      send(post);
    }

    return true;
  }

}}
