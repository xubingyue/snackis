#include <cassert>
#include "snackis/ctx.hpp"
#include "snackis/gui/gui.hpp"
#include "snackis/gui/post_search.hpp"
#include "snackis/gui/post_view.hpp"

namespace snackis {
namespace gui {
  enum FeedCol {COL_FEED_PTR=0, COL_FEED_ID, COL_FEED_NAME};
  enum PostCol {COL_PTR=0, COL_BY, COL_FEED, COL_BODY};

  static void on_find(gpointer *_, PostSearch *v) {
    Ctx &ctx(v->ctx);
    gtk_list_store_clear(v->posts);
    size_t cnt(0);
    
    auto feed_sel(get_sel_rec<Feed>(GTK_COMBO_BOX(v->feed_fld)));
    str body_sel(trim(gtk_entry_get_text(GTK_ENTRY(v->body))));
    auto peer_sel(v->peer_fld.selected);
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

    for (auto key = ctx.db.posts_sort.recs.begin();
	 key != ctx.db.posts_sort.recs.end();
	 key++) {
      auto &rec(db::get(ctx.db.posts, *key));
      Post post(ctx, rec);
      Feed feed(get_feed_id(ctx, post.feed_id));
      
      if (peer_sel) {
	if (post.owner_id != peer_sel->id) { continue; }
      }      

      if (feed_sel) {
	if (feed.id != *db::get(*feed_sel, ctx.db.feed_id)) {
	  continue;
	}
      } else {
	if (!feed.visible) { continue; }
      }

      if (!body_sel.empty() && find_ci(post.body, body_sel) == str::npos) {
	continue;
      }

      if (min_time_sel && post.created_at < *min_time_sel) {
	continue;
      }

      if (max_time_sel && post.created_at > *max_time_sel) {
	continue;
      }
      
      Peer peer(get_peer_id(ctx, post.owner_id));
      GtkTreeIter iter;
      gtk_list_store_append(v->posts, &iter);
      const str by(fmt("%0\n%1",
		       peer.name.c_str(),
		       fmt(post.created_at, "%a %b %d, %H:%M").c_str()));
      gtk_list_store_set(v->posts, &iter,
			 COL_PTR, &rec,
			 COL_BY, by.c_str(),
			 COL_FEED, feed.name.c_str(),
			 COL_BODY, post.body.c_str(),
			 -1);
      cnt++;
    }

    gtk_widget_grab_focus(cnt ? v->list : v->feed_fld);
  }

  static void on_edit(GtkTreeView *treeview,
		      GtkTreePath *path,
		      GtkTreeViewColumn *col,
		      PostSearch *v) {
    Ctx &ctx(v->ctx);
    auto post_rec(get_sel_rec<Post>(GTK_TREE_VIEW(v->list)));
    assert(post_rec);
    Post post(ctx, *post_rec);    
    PostView *fv(new PostView(post));
    push_view(*fv);
  }
  
  static void on_close(gpointer *_, PostSearch *v) {
    pop_view(*v);
  }
  
  static void init_feeds(PostSearch &v) {
    Ctx &ctx(v.ctx);
    
    GtkTreeIter iter;
    gtk_list_store_append(v.feed_store, &iter);
    gtk_list_store_set(v.feed_store, &iter,
		       COL_FEED_PTR, nullptr,
		       COL_FEED_ID, "",
		       COL_FEED_NAME, "",
		       -1);

    for(auto key = ctx.db.feeds_sort.recs.begin();
	key != ctx.db.feeds_sort.recs.end();
	key++) {
      auto &rec(db::get(ctx.db.feeds, *key));
      Feed feed(ctx, rec);
      
      if ((feed.visible && feed.active) || (v.feed && v.feed->id == feed.id)) {
	gtk_list_store_append(v.feed_store, &iter);
	gtk_list_store_set(v.feed_store, &iter,
			   COL_FEED_PTR, &rec,
			   COL_FEED_ID, to_str(feed.id).c_str(),
			   COL_FEED_NAME, feed.name.c_str(),
			   -1);
      }
    }

    if (v.feed) {
      gtk_combo_box_set_active_id(GTK_COMBO_BOX(v.feed_fld),
				  to_str(v.feed->id).c_str());
    } else {
      gtk_combo_box_set_active(GTK_COMBO_BOX(v.feed_fld), 0);
    }    
  }

  static void init_list(PostSearch &v) {
    gtk_widget_set_hexpand(v.list, true);
    add_col(GTK_TREE_VIEW(v.list), "Posted", COL_BY);
    add_col(GTK_TREE_VIEW(v.list), "Feed", COL_FEED, true);
    add_col(GTK_TREE_VIEW(v.list), "Body", COL_BODY, true);
    g_signal_connect(v.list, "row-activated", G_CALLBACK(on_edit), &v);
  }
  
  PostSearch::PostSearch(Ctx &ctx):
    View(ctx, "Post Search"),
    feed_store(gtk_list_store_new(3, G_TYPE_POINTER, G_TYPE_STRING, G_TYPE_STRING)),
    posts(gtk_list_store_new(4,
			     G_TYPE_POINTER,
			     G_TYPE_STRING, G_TYPE_STRING, G_TYPE_STRING)),
    body(gtk_entry_new()),
    min_time(gtk_entry_new()),
    max_time(gtk_entry_new()),
    feed_fld(new_combo_box(GTK_TREE_MODEL(feed_store))),
    find(gtk_button_new_with_mnemonic("_Find Posts")),
    list(gtk_tree_view_new_with_model(GTK_TREE_MODEL(posts))),
    close(gtk_button_new_with_mnemonic("_Close Search")),
    peer_fld(ctx)
  { }

  void PostSearch::init() {
    GtkWidget *lbl;

    GtkWidget *frm = gtk_box_new(GTK_ORIENTATION_VERTICAL, 5);
    gtk_box_pack_start(GTK_BOX(panel), frm, false, false, 0);

    GtkWidget *post_box(gtk_grid_new());
    gtk_grid_set_row_spacing(GTK_GRID(post_box), 5);
    gtk_grid_set_column_spacing(GTK_GRID(post_box), 5);
    gtk_container_add(GTK_CONTAINER(frm), post_box);

    lbl = gtk_label_new("Body");
    gtk_widget_set_halign(lbl, GTK_ALIGN_START);
    gtk_grid_attach(GTK_GRID(post_box), lbl, 0, 0, 1, 1);
    gtk_widget_set_hexpand(body, true);
    gtk_grid_attach(GTK_GRID(post_box), body, 0, 1, 1, 1);

    lbl = gtk_label_new("Posted");
    gtk_widget_set_halign(lbl, GTK_ALIGN_START);
    gtk_grid_attach(GTK_GRID(post_box), lbl, 1, 0, 3, 1);
    gtk_entry_set_placeholder_text(GTK_ENTRY(min_time),
				   "yyyy-mm-dd hh:mm");
    gtk_grid_attach(GTK_GRID(post_box), min_time, 1, 1, 1, 1);
    gtk_entry_set_text(GTK_ENTRY(min_time),
		       fmt(now() - std::chrono::hours(7), "%Y-%m-%d %H:%M").c_str());
    gtk_grid_attach(GTK_GRID(post_box), gtk_label_new("-"), 2, 1, 1, 1);
    gtk_entry_set_placeholder_text(GTK_ENTRY(max_time),
				   "yyyy-mm-dd hh:mm");
    gtk_grid_attach(GTK_GRID(post_box), max_time, 3, 1, 1, 1);

    init_feeds(*this);
    lbl = gtk_label_new("Feed");
    gtk_widget_set_halign(lbl, GTK_ALIGN_START);
    gtk_container_add(GTK_CONTAINER(frm), lbl);
    gtk_widget_set_hexpand(feed_fld, true);
    gtk_container_add(GTK_CONTAINER(frm), feed_fld);
    
    lbl = gtk_label_new("By");
    gtk_widget_set_halign(lbl, GTK_ALIGN_START);
    gtk_container_add(GTK_CONTAINER(frm), lbl);
    gtk_container_add(GTK_CONTAINER(frm), peer_fld.ptr());
    
    gtk_widget_set_halign(find, GTK_ALIGN_END);
    g_signal_connect(find, "clicked", G_CALLBACK(on_find), this);
    gtk_widget_set_margin_top(find, 5);
    gtk_container_add(GTK_CONTAINER(frm), find);

    init_list(*this);
    gtk_box_pack_start(GTK_BOX(panel), list, true, true, 0);
    lbl = gtk_label_new("Press Return or double-click to edit post");
    gtk_container_add(GTK_CONTAINER(panel), lbl);

    GtkWidget *btns = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 5);
    gtk_widget_set_halign(btns, GTK_ALIGN_END);
    gtk_widget_set_valign(btns, GTK_ALIGN_END);
    gtk_widget_set_margin_top(btns, 10);
    gtk_container_add(GTK_CONTAINER(panel), btns);
        
    g_signal_connect(close, "clicked", G_CALLBACK(on_close), this);
    gtk_container_add(GTK_CONTAINER(btns), close);
    focused = feed_fld;
  }
}}
