#include <cassert>
#include "snackis/ctx.hpp"
#include "snackis/gui/gui.hpp"
#include "snackis/gui/post_search.hpp"
#include "snackis/gui/post_view.hpp"

namespace snackis {
namespace gui {
  enum PeerCol {COL_PEER_PTR=0, COL_PEER_NAME};
  enum PostCol {COL_PTR=0, COL_BY, COL_FEED, COL_BODY};

  static void on_find(gpointer *_, PostSearch *v) {
    Ctx &ctx(v->ctx);
    gtk_list_store_clear(v->posts);
    size_t cnt(0);
    
    str feed_name_sel(trim(gtk_entry_get_text(GTK_ENTRY(v->feed_name))));
    str body_sel(trim(gtk_entry_get_text(GTK_ENTRY(v->body))));
    auto peer_sel(get_sel_rec<Peer>(GTK_COMBO_BOX(v->peer)));
    str min_time(trim(gtk_entry_get_text(GTK_ENTRY(v->min_time))));
    auto min_time_sel(parse_time("%Y-%m-%d %H:%M", min_time));
    
    if (!min_time.empty() && !min_time_sel) {
      log(ctx, fmt("Failed parsing time: '%0'", min_time));
      gtk_widget_grab_focus(v->min_time);
      return;
    }

    str max_time(trim(gtk_entry_get_text(GTK_ENTRY(v->max_time))));
    auto max_time_sel(parse_time("%Y-%m-%d %H:%M", max_time));
    
    if (!max_time.empty() && !max_time_sel) {
      log(ctx, fmt("Failed parsing time: '%0'", max_time));
      gtk_widget_grab_focus(v->max_time);
      return;
    }
    
    bool active_sel(gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(v->active)));
    
    for (auto rec = ctx.db.at_posts.recs.rbegin();
	 rec != ctx.db.at_posts.recs.rend();
	 rec++) {
      Post post(ctx, *rec);
      db::load(ctx.db.posts, post);
      Feed feed(get_feed_id(ctx, post.feed_id));
      
      if (feed.active != active_sel) { continue; }

      if (peer_sel) {
	auto peer_id(*db::get(*peer_sel, ctx.db.peer_id));
	if (post.by_id != peer_id) { continue; }
      }      

      if (!feed_name_sel.empty() && find_ci(feed.name, feed_name_sel) == str::npos) {
	continue;
      }

      if (!body_sel.empty() && find_ci(post.body, body_sel) == str::npos) {
	continue;
      }

      if (min_time_sel && post.at < *min_time_sel) {
	continue;
      }

      if (max_time_sel && post.at > *max_time_sel) {
	continue;
      }
      
      Peer peer(get_peer_id(ctx, post.by_id));
      GtkTreeIter iter;
      gtk_list_store_append(v->posts, &iter);
      const str by(fmt("%0\n%1",
		       peer.name.c_str(),
		       fmt(post.at, "%a %b %d, %H:%M:%S").c_str()));
      gtk_list_store_set(v->posts, &iter,
			 COL_PTR, &rec,
			 COL_BY, by.c_str(),
			 COL_FEED, feed.name.c_str(),
			 COL_BODY, post.body.c_str(),
			 -1);
      cnt++;
    }

    gtk_widget_grab_focus(cnt ? v->list : v->feed_name);
  }

  static void on_edit(GtkTreeView *treeview,
		      GtkTreePath *path,
		      GtkTreeViewColumn *col,
		      PostSearch *v) {
    Ctx &ctx(v->ctx);
    auto post_rec(get_sel_rec<Post>(GTK_TREE_VIEW(v->list)));
    assert(post_rec);

    Post post(ctx, *post_rec);
    if (post.by_id == whoami(ctx).id) {
      PostView *fv(new PostView(post));
      push_view(*fv);
    }
  }
  
  static void on_close(gpointer *_, PostSearch *v) {
    pop_view(*v);
  }
  
  static void init_peers(PostSearch &v) {
    GtkTreeIter iter;
    gtk_list_store_append(v.peers, &iter);
    gtk_list_store_set(v.peers, &iter,
		       COL_PEER_PTR, nullptr,
		       COL_PEER_NAME, "",
		       -1);

    for (auto &rec: v.ctx.db.peers.recs) {
      gtk_list_store_append(v.peers, &iter);
      gtk_list_store_set(v.peers, &iter,
			 COL_PEER_PTR, &rec,
			 COL_PEER_NAME, db::get(rec, v.ctx.db.peer_name)->c_str(),
			 -1);
    }
  }

  static void init_list(PostSearch &v) {
    gtk_widget_set_hexpand(v.list, true);
    auto rend(gtk_cell_renderer_text_new());
    auto col(gtk_tree_view_column_new_with_attributes("Results",
						      rend,
						      "text", COL_BY,
						      nullptr));
    gtk_tree_view_append_column(GTK_TREE_VIEW(v.list), col);

    rend = gtk_cell_renderer_text_new();
    col = gtk_tree_view_column_new_with_attributes("",
						   rend,
						   "text", COL_FEED,
						   nullptr);
    gtk_tree_view_column_set_expand(GTK_TREE_VIEW_COLUMN(col), true);
    gtk_tree_view_append_column(GTK_TREE_VIEW(v.list), col);

    rend = gtk_cell_renderer_text_new();
    col = gtk_tree_view_column_new_with_attributes("",
						   rend,
						   "text", COL_BODY,
						   nullptr);
    gtk_tree_view_column_set_expand(GTK_TREE_VIEW_COLUMN(col), true);
    gtk_tree_view_append_column(GTK_TREE_VIEW(v.list), col);
    g_signal_connect(v.list, "row-activated", G_CALLBACK(on_edit), &v);
  }
  
  PostSearch::PostSearch(Ctx &ctx):
    View(ctx, "Post Search"),
    peers(gtk_list_store_new(2, G_TYPE_POINTER, G_TYPE_STRING)),
    posts(gtk_list_store_new(4,
			     G_TYPE_POINTER,
			     G_TYPE_STRING, G_TYPE_STRING, G_TYPE_STRING)),
    feed_name(gtk_entry_new()),
    active(gtk_check_button_new_with_label("Active")),
    body(gtk_entry_new()),
    min_time(gtk_entry_new()),
    max_time(gtk_entry_new()),
    peer(gtk_combo_box_new_with_model(GTK_TREE_MODEL(peers))),
    find(gtk_button_new_with_mnemonic("_Find Posts")),
    list(gtk_tree_view_new_with_model(GTK_TREE_MODEL(posts))),
    close(gtk_button_new_with_mnemonic("_Close Search")) {
    GtkWidget *lbl;

    GtkWidget *frm = gtk_box_new(GTK_ORIENTATION_VERTICAL, 5);
    gtk_box_pack_start(GTK_BOX(panel), frm, false, false, 0);

    lbl = gtk_label_new("Feed Name");
    gtk_widget_set_halign(lbl, GTK_ALIGN_START);
    gtk_container_add(GTK_CONTAINER(frm), lbl);
    GtkWidget *feed_box(gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 5));
    gtk_container_add(GTK_CONTAINER(frm), feed_box);
    gtk_widget_set_hexpand(feed_name, true);
    gtk_container_add(GTK_CONTAINER(feed_box), feed_name);
    gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(active), true);
    gtk_container_add(GTK_CONTAINER(feed_box), active);

    lbl = gtk_label_new("Body");
    gtk_widget_set_halign(lbl, GTK_ALIGN_START);
    gtk_container_add(GTK_CONTAINER(frm), lbl);
    gtk_widget_set_hexpand(body, true);
    gtk_container_add(GTK_CONTAINER(frm), body);

    init_peers(*this);

    GtkWidget *post_box(gtk_grid_new());
    gtk_grid_set_row_spacing(GTK_GRID(post_box), 5);
    gtk_grid_set_column_spacing(GTK_GRID(post_box), 5);
    gtk_container_add(GTK_CONTAINER(frm), post_box);

    lbl = gtk_label_new("Posted");
    gtk_widget_set_halign(lbl, GTK_ALIGN_START);
    gtk_grid_attach(GTK_GRID(post_box), lbl, 0, 0, 3, 1);
    gtk_entry_set_text(GTK_ENTRY(min_time),
		       fmt(now() - std::chrono::hours(7), "%Y-%m-%d %H:%M").c_str());
    gtk_grid_attach(GTK_GRID(post_box), min_time, 0, 1, 1, 1);
    gtk_grid_attach(GTK_GRID(post_box), gtk_label_new("-"), 1, 1, 1, 1);
    gtk_grid_attach(GTK_GRID(post_box), max_time, 2, 1, 1, 1);

    lbl = gtk_label_new("By");
    gtk_widget_set_halign(lbl, GTK_ALIGN_START);
    gtk_grid_attach(GTK_GRID(post_box), lbl, 3, 0, 1, 1);

    auto rend(gtk_cell_renderer_text_new());
    gtk_cell_layout_pack_start(GTK_CELL_LAYOUT(peer), rend, true);
    gtk_cell_layout_set_attributes(GTK_CELL_LAYOUT(peer),
				   rend,
                                   "text", COL_PEER_NAME,
				   nullptr);
    gtk_widget_set_hexpand(peer, true);
    gtk_grid_attach(GTK_GRID(post_box), peer, 3, 1, 1, 1);
    
    gtk_widget_set_halign(find, GTK_ALIGN_END);
    g_signal_connect(find, "clicked", G_CALLBACK(on_find), this);
    gtk_widget_set_margin_top(find, 5);
    gtk_container_add(GTK_CONTAINER(frm), find);

    init_list(*this);
    gtk_widget_set_margin_top(list, 5);
    gtk_box_pack_start(GTK_BOX(panel), list, true, true, 0);
    lbl = gtk_label_new("Press Return or double-click to edit selected post");
    gtk_container_add(GTK_CONTAINER(panel), lbl);

    GtkWidget *btns = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 5);
    gtk_widget_set_halign(btns, GTK_ALIGN_END);
    gtk_widget_set_valign(btns, GTK_ALIGN_END);
    gtk_widget_set_margin_top(btns, 10);
    gtk_container_add(GTK_CONTAINER(panel), btns);
        
    g_signal_connect(close, "clicked", G_CALLBACK(on_close), this);
    gtk_container_add(GTK_CONTAINER(btns), close);
    focused = feed_name;
  }
}}
