#include <cassert>
#include "snackis/ctx.hpp"
#include "snackis/gui/gui.hpp"
#include "snackis/gui/feed_view.hpp"
#include "snackis/gui/post_view.hpp"

namespace snackis {
namespace gui {
  enum PeerCol {COL_PEER_PTR=0, COL_PEER_NAME};
  enum PostCol {COL_POST_PTR=0, COL_POST_BY, COL_POST_BODY};
  
  static void on_cancel(gpointer *_, FeedView *v) {
    log(v->ctx, "Cancelled feed");
    pop_view(*v);
  }
  
  static void on_save(gpointer *_, FeedView *v) {
    Ctx &ctx(v->ctx);
    db::Trans trans(ctx);
    v->feed.name = gtk_entry_get_text(GTK_ENTRY(v->name));
    v->feed.active = gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(v->active));
    db::upsert(ctx.db.feeds, v->feed);
    const str post_body(get_str(GTK_TEXT_VIEW(v->new_post_text)));

    if (!post_body.empty()) {
      Post post(ctx);
      post.feed_id = v->feed.id;
      post.body = post_body;
      db::insert(ctx.db.posts, post);
      send(post);
    }
    
    db::commit(trans);
    log(v->ctx, "Saved feed");
    pop_view(*v);
  }

  static void on_peer_list_sel(gpointer *_, FeedView *v) {
    auto sel(gtk_tree_view_get_selection(GTK_TREE_VIEW(v->peer_list)));
    auto cnt(gtk_tree_selection_count_selected_rows(sel));
    gtk_widget_set_sensitive(v->remove_peers, cnt > 0);
  }
  
  static void on_peer_sel(gpointer *_, FeedView *v) {
    Ctx &ctx(v->ctx);
    db::Rec<Peer> *rec(get_sel_rec<Peer>(GTK_COMBO_BOX(v->peer)));
    auto id(*db::get(*rec, ctx.db.peer_id));
    bool exists = v->feed.peer_ids.find(id) != v->feed.peer_ids.end();
    gtk_widget_set_sensitive(v->add_peer, !exists);
    gtk_widget_set_sensitive(v->remove_peers, exists);
  }
  
  static void on_add_peer(gpointer *_, FeedView *v) {
    Ctx &ctx(v->ctx);
    db::Rec<Peer> *rec(get_sel_rec<Peer>(GTK_COMBO_BOX(v->peer)));

    GtkTreeIter iter;    
    gtk_list_store_append(v->feed_peers, &iter);
    gtk_list_store_set(v->feed_peers, &iter,
		       COL_PEER_PTR, rec,
		       COL_PEER_NAME, db::get(*rec, ctx.db.peer_name)->c_str(),
		       -1);
    auto sel(gtk_tree_view_get_selection(GTK_TREE_VIEW(v->peer_list)));
    gtk_tree_selection_select_iter(sel, &iter);
  
    v->feed.peer_ids.insert(*db::get(*rec, ctx.db.peer_id));
    gtk_widget_set_sensitive(v->add_peer, false);
    gtk_widget_set_sensitive(v->remove_peers, true);
    gtk_widget_grab_focus(v->peer);
  }

  static void on_remove_peers(gpointer *_, FeedView *v) {
    Ctx &ctx(v->ctx);
    
    GtkTreeIter iter;    
    gtk_tree_model_get_iter_first(GTK_TREE_MODEL(v->feed_peers), &iter);
    auto sel(gtk_tree_view_get_selection(GTK_TREE_VIEW(v->peer_list)));
    
    while (true) {
      db::Rec<Peer> *iter_rec;
      gtk_tree_model_get(GTK_TREE_MODEL(v->feed_peers),
			 &iter,
			 COL_PEER_PTR, &iter_rec,
			 -1);
      if (gtk_tree_selection_iter_is_selected(sel, &iter)) {
	gtk_list_store_remove(v->feed_peers, &iter);
	v->feed.peer_ids.erase(*db::get(*iter_rec, ctx.db.peer_id));

	if (!gtk_list_store_iter_is_valid(v->feed_peers, &iter)) {
	  break;
	}
      } else if (!gtk_tree_model_iter_next(GTK_TREE_MODEL(v->feed_peers), &iter)) {
	break;
      }
    }
    
    gtk_widget_set_sensitive(v->add_peer, true);
    gtk_widget_set_sensitive(v->remove_peers, false);    
  }
  
  static void load_peers(FeedView &v) {
    Ctx &ctx(v.ctx);
    
    for (const auto &peer_id: v.feed.peer_ids) {
      db::Rec<Peer> key;
      db::set(key, ctx.db.peer_id, peer_id);
      const db::Rec<Peer> &rec(db::get(ctx.db.peers, key));
      
      GtkTreeIter iter;
      gtk_list_store_append(v.feed_peers, &iter);
      gtk_list_store_set(v.feed_peers, &iter,
			 COL_PEER_PTR, &rec,
			 COL_PEER_NAME, db::get(rec, ctx.db.peer_name)->c_str(),
			 -1);
    }
  }
  
  static void init_peers(FeedView &v) {
    Ctx &ctx(v.ctx);

    gtk_widget_set_hexpand(v.peer_list, true);
    auto rend(gtk_cell_renderer_text_new());
    auto name_col(gtk_tree_view_column_new_with_attributes("Peers",
							   rend,
							   "text", COL_PEER_NAME,
							   nullptr));
    gtk_tree_view_append_column(GTK_TREE_VIEW(v.peer_list), name_col);    
    auto sel(gtk_tree_view_get_selection(GTK_TREE_VIEW(v.peer_list)));
    gtk_tree_selection_set_mode(sel, GTK_SELECTION_MULTIPLE);
    g_signal_connect(sel, "changed", G_CALLBACK(on_peer_list_sel), &v);
    
    for(const auto &peer_rec: ctx.db.peers.recs) {
      Peer peer(ctx, peer_rec);
      
      GtkTreeIter iter;
      gtk_list_store_append(v.peers, &iter);
      gtk_list_store_set(v.peers, &iter,
			 COL_PEER_PTR, &peer_rec,
			 COL_PEER_NAME, peer.name.c_str(),
			 -1);
    }

    g_signal_connect(v.remove_peers, "clicked", G_CALLBACK(on_remove_peers), &v);

    rend = gtk_cell_renderer_text_new();
    gtk_cell_layout_pack_start(GTK_CELL_LAYOUT(v.peer), rend, true);
    gtk_cell_layout_set_attributes(GTK_CELL_LAYOUT(v.peer),
				   rend,
                                   "text", COL_PEER_NAME,
				   nullptr);
    gtk_widget_set_hexpand(v.peer, true);
    g_signal_connect(v.peer, "changed", G_CALLBACK(on_peer_sel), &v);
    gtk_combo_box_set_active(GTK_COMBO_BOX(v.peer), 0);
    gtk_container_add(GTK_CONTAINER(v.peer_input), v.peer);

    g_signal_connect(v.add_peer, "clicked", G_CALLBACK(on_add_peer), &v);
    gtk_container_add(GTK_CONTAINER(v.peer_input), v.add_peer);
    load_peers(v);
  }

  static void on_edit_post(GtkTreeView *treeview,
			   GtkTreePath *path,
			   GtkTreeViewColumn *col,
			   FeedView *v) {
    Ctx &ctx(v->ctx);
    auto post_rec(get_sel_rec<Post>(GTK_TREE_VIEW(v->post_list)));
    assert(post_rec);
    Post post(ctx, *post_rec);

    if (post.by_id == whoami(ctx).id) {
      PostView *pv(new PostView(post));
      push_view(*pv);
    }
  }
  
  static void load_posts(FeedView &v) {
    Ctx &ctx(v.ctx);

    for (auto rec: last_posts(v.feed, Time::max(), 7)) {
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
  }

  static void init_posts(FeedView &v) {
    gtk_widget_set_hexpand(v.post_list, true);
    gtk_widget_set_vexpand(v.post_list, true);
    auto rend(gtk_cell_renderer_text_new());
    auto by_col(gtk_tree_view_column_new_with_attributes("Post History",
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
  
  FeedView::FeedView(const Feed &feed):
    View(feed.ctx, "Feed"),
    feed(feed),
    peers(gtk_list_store_new(2, G_TYPE_POINTER, G_TYPE_STRING)),
    feed_peers(gtk_list_store_new(2, G_TYPE_POINTER, G_TYPE_STRING)),
    posts(gtk_list_store_new(3,
			     G_TYPE_POINTER,
			     G_TYPE_STRING, G_TYPE_STRING)),
    name(gtk_entry_new()),
    active(gtk_check_button_new_with_label("Active")),
    peer_list(gtk_tree_view_new_with_model(GTK_TREE_MODEL(feed_peers))),
    remove_peers(gtk_button_new_with_mnemonic("_Remove Selected Peers")),
    peer_input(gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 5)),
    peer(gtk_combo_box_new_with_model(GTK_TREE_MODEL(peers))),
    add_peer(gtk_button_new_with_mnemonic("_Add Peer")),
    post_list(gtk_tree_view_new_with_model(GTK_TREE_MODEL(posts))),
    new_post_text(gtk_text_view_new()),
    new_post(gtk_scrolled_window_new(NULL, NULL)),
    save(gtk_button_new_with_mnemonic("_Save Feed")),
    cancel(gtk_button_new_with_mnemonic("_Cancel")) {
    GtkWidget *lbl;

    GtkWidget *frm = gtk_box_new(GTK_ORIENTATION_VERTICAL, 5);
    gtk_box_pack_start(GTK_BOX(panel), frm, true, true, 0);
    
    lbl = gtk_label_new("Name");
    gtk_widget_set_halign(lbl, GTK_ALIGN_START);
    gtk_container_add(GTK_CONTAINER(frm), lbl);
    gtk_widget_set_hexpand(name, true);
    gtk_container_add(GTK_CONTAINER(frm), name);
    gtk_entry_set_text(GTK_ENTRY(name), feed.name.c_str());

    GtkWidget *active_box(gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 0));
    gtk_container_add(GTK_CONTAINER(frm), active_box);
    gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(active), feed.active);
    gtk_container_add(GTK_CONTAINER(active_box), active);

    init_peers(*this);
    gtk_widget_set_margin_top(peer_list, 5);
    gtk_container_add(GTK_CONTAINER(frm), peer_list);
    GtkWidget *peer_btns = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 5);
    gtk_container_add(GTK_CONTAINER(frm), peer_btns);
    gtk_container_add(GTK_CONTAINER(peer_btns), remove_peers);
    gtk_widget_set_margin_top(peer_input, 5);
    gtk_container_add(GTK_CONTAINER(frm), peer_input);

    init_posts(*this);
    gtk_widget_set_margin_top(post_list, 5);
    gtk_container_add(GTK_CONTAINER(frm), post_list);
    lbl = gtk_label_new("Press Return or double-click to edit selected post");
    gtk_container_add(GTK_CONTAINER(frm), lbl);

    lbl = gtk_label_new("New Post");
    gtk_widget_set_halign(lbl, GTK_ALIGN_START);
    gtk_widget_set_margin_top(lbl, 5);
    gtk_container_add(GTK_CONTAINER(frm), lbl);
    gtk_widget_set_hexpand(new_post_text, true);
    gtk_widget_set_vexpand(new_post_text, true);
    gtk_text_view_set_wrap_mode(GTK_TEXT_VIEW(new_post_text), GTK_WRAP_WORD);
    gtk_scrolled_window_set_overlay_scrolling(GTK_SCROLLED_WINDOW(new_post),
					      false);
    gtk_scrolled_window_set_policy(GTK_SCROLLED_WINDOW(new_post),
				   GTK_POLICY_NEVER,
				   GTK_POLICY_ALWAYS);
    gtk_container_add(GTK_CONTAINER(new_post), new_post_text);
    gtk_container_add(GTK_CONTAINER(frm), new_post);
    
    GtkWidget *btns = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 5);
    gtk_widget_set_halign(btns, GTK_ALIGN_END);
    gtk_widget_set_valign(btns, GTK_ALIGN_END);
    gtk_widget_set_margin_top(btns, 10);
    gtk_container_add(GTK_CONTAINER(panel), btns);
        
    g_signal_connect(save, "clicked", G_CALLBACK(on_save), this);
    gtk_container_add(GTK_CONTAINER(btns), save);

    g_signal_connect(cancel, "clicked", G_CALLBACK(on_cancel), this);
    gtk_container_add(GTK_CONTAINER(btns), cancel);
    focused = name;
  }
}}
