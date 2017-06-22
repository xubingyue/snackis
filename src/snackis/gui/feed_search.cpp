#include <cassert>
#include "snackis/ctx.hpp"
#include "snackis/gui/gui.hpp"
#include "snackis/gui/feed_search.hpp"
#include "snackis/gui/feed_view.hpp"

namespace snackis {
namespace gui {
  enum PeerCol {COL_PEER_PTR=0, COL_PEER_NAME};
  enum FeedCol {COL_FEED_PTR=0, COL_FEED_NAME};

  static void on_find(gpointer *_, FeedSearch *v) {
    Ctx &ctx(v->ctx);
    gtk_list_store_clear(v->feeds);
    size_t cnt(0);
    
    str name_sel(trim(gtk_entry_get_text(GTK_ENTRY(v->name))));
    auto peer_sel(get_sel_rec<Peer>(GTK_COMBO_BOX(v->peer)));
    bool active_sel(gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(v->active)));
    
    for (auto &rec: ctx.db.feeds.recs) {
      Feed feed(ctx, rec);

      if (feed.active != active_sel) { continue; }

      if (peer_sel) {
	auto peer_id(*db::get(*peer_sel, ctx.db.peer_id));
	if (feed.peer_ids.find(peer_id) == feed.peer_ids.end()) { continue; }
      }      

      if (!name_sel.empty() && find_ci(feed.name, name_sel) == str::npos) {
	continue;
      }
      
      GtkTreeIter iter;
      gtk_list_store_append(v->feeds, &iter);
      gtk_list_store_set(v->feeds, &iter,
			 COL_FEED_PTR, &rec,
			 COL_FEED_NAME, db::get(rec, ctx.db.feed_name)->c_str(),
			 -1);
      cnt++;
    }

    gtk_widget_grab_focus(cnt ? v->list : v->peer);
  }

  static db::Rec<Feed> *get_sel_feed(FeedSearch &v) {
    GtkTreeSelection *sel = gtk_tree_view_get_selection(GTK_TREE_VIEW(v.list));
    GtkTreeIter iter;
    if (!gtk_tree_selection_get_selected(sel, nullptr, &iter)) { return nullptr; }
    db::Rec<Feed> *rec = nullptr;
    gtk_tree_model_get(GTK_TREE_MODEL(v.feeds), &iter, COL_FEED_PTR, &rec, -1);
    return rec;
  }

  static void on_edit(GtkTreeView *treeview,
		      GtkTreePath *path,
		      GtkTreeViewColumn *col,
		      FeedSearch *v) {
    Ctx &ctx(v->ctx);
    auto feed_rec(get_sel_feed(*v));
    assert(feed_rec);
    Feed feed(ctx, *feed_rec);
    FeedView *fv(new FeedView(feed));
    push_view(*fv);
  }
  
  static void on_close(gpointer *_, FeedSearch *v) {
    pop_view(*v);
  }
  
  static void init_peers(FeedSearch &v) {
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

  static void init_list(FeedSearch &v) {
    gtk_widget_set_hexpand(v.list, true);
    auto rend(gtk_cell_renderer_text_new());
    auto name_col(gtk_tree_view_column_new_with_attributes("Results",
							   rend,
							   "text", COL_FEED_NAME,
							   nullptr));
    gtk_tree_view_append_column(GTK_TREE_VIEW(v.list), name_col);
    g_signal_connect(v.list, "row-activated", G_CALLBACK(on_edit), &v);
  }
  
  FeedSearch::FeedSearch(Ctx &ctx):
    View(ctx, "Feed Search"),
    peers(gtk_list_store_new(2, G_TYPE_POINTER, G_TYPE_STRING)),
    feeds(gtk_list_store_new(2, G_TYPE_POINTER, G_TYPE_STRING)),
    name(gtk_entry_new()),
    peer(gtk_combo_box_new_with_model(GTK_TREE_MODEL(peers))),
    active(gtk_check_button_new_with_label("Active")),
    find(gtk_button_new_with_mnemonic("_Find Feeds")),
    list(gtk_tree_view_new_with_model(GTK_TREE_MODEL(feeds))),
    close(gtk_button_new_with_mnemonic("_Close Search")) {
    GtkWidget *lbl;

    GtkWidget *frm = gtk_box_new(GTK_ORIENTATION_VERTICAL, 5);
    gtk_box_pack_start(GTK_BOX(panel), frm, false, false, 0);

    lbl = gtk_label_new("Name");
    gtk_widget_set_halign(lbl, GTK_ALIGN_START);
    gtk_container_add(GTK_CONTAINER(frm), lbl);
    gtk_widget_set_hexpand(name, true);
    gtk_container_add(GTK_CONTAINER(frm), name);
    
    init_peers(*this);
    lbl = gtk_label_new("Peer");
    gtk_widget_set_halign(lbl, GTK_ALIGN_START);
    gtk_container_add(GTK_CONTAINER(frm), lbl);

    auto rend(gtk_cell_renderer_text_new());
    gtk_cell_layout_pack_start(GTK_CELL_LAYOUT(peer), rend, true);
    gtk_cell_layout_set_attributes(GTK_CELL_LAYOUT(peer),
				   rend,
                                   "text", COL_PEER_NAME,
				   nullptr);
    gtk_widget_set_hexpand(peer, true);
    gtk_container_add(GTK_CONTAINER(frm), peer);
    
    GtkWidget *active_box(gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 0));
    gtk_container_add(GTK_CONTAINER(frm), active_box);
    gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(active), true);
    gtk_container_add(GTK_CONTAINER(active_box), active);

    gtk_widget_set_halign(find, GTK_ALIGN_END);
    g_signal_connect(find, "clicked", G_CALLBACK(on_find), this);
    gtk_container_add(GTK_CONTAINER(frm), find);

    init_list(*this);
    gtk_widget_set_margin_top(list, 5);
    gtk_box_pack_start(GTK_BOX(panel), list, true, true, 0);
    lbl = gtk_label_new("Press Return or double-click to edit selected feed");
    gtk_container_add(GTK_CONTAINER(panel), lbl);

    GtkWidget *btns = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 5);
    gtk_widget_set_halign(btns, GTK_ALIGN_END);
    gtk_widget_set_valign(btns, GTK_ALIGN_END);
    gtk_widget_set_margin_top(btns, 10);
    gtk_container_add(GTK_CONTAINER(panel), btns);
        
    g_signal_connect(close, "clicked", G_CALLBACK(on_close), this);
    gtk_container_add(GTK_CONTAINER(btns), close);
    focused = name;
  }
}}
