#include <cassert>
#include "snackis/ctx.hpp"
#include "snackis/gui/gui.hpp"
#include "snackis/gui/feed_search.hpp"
#include "snackis/gui/feed_view.hpp"

namespace snackis {
namespace gui {
  enum PeerCol {COL_PEER_PTR=0, COL_PEER_ID, COL_PEER_NAME};
  enum FeedCol {COL_FEED_PTR=0, COL_FEED_CREATED, COL_FEED_OWNER, COL_FEED_NAME};

  static void on_find(gpointer *_, FeedSearch *v) {
    Ctx &ctx(v->ctx);
    gtk_list_store_clear(v->feeds);
    size_t cnt(0);
    
    str name_sel(trim(gtk_entry_get_text(GTK_ENTRY(v->name))));
    auto &peer_sel(v->peer_fld.selected);
    bool active_sel(gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(v->active)));
    
    for (auto key = ctx.db.feeds_sort.recs.begin();
	 key != ctx.db.feeds_sort.recs.end();
	 key++) {
      auto &rec(db::get(ctx.db.feeds, *key));
      Feed feed(ctx, rec);

      if (!feed.visible) { continue; }
      if (feed.active != active_sel) { continue; }

      if (peer_sel) {
	auto peer_id(*db::get(*peer_sel, ctx.db.peer_id));
	if (feed.owner_id != peer_id) { continue; }
      }      

      if (!name_sel.empty() && find_ci(feed.name, name_sel) == str::npos) {
	continue;
      }
      
      Peer own(get_peer_id(ctx, feed.owner_id));

      GtkTreeIter iter;
      gtk_list_store_append(v->feeds, &iter);
      gtk_list_store_set(v->feeds, &iter,
			 COL_FEED_PTR, &rec,
			 COL_FEED_CREATED,
			 fmt(feed.created_at, "%a %b %d, %H:%M").c_str(),
			 COL_FEED_OWNER, own.name.c_str(),
			 COL_FEED_NAME, feed.name.c_str(),
			 -1);
      cnt++;
    }

    gtk_widget_grab_focus(cnt ? v->list : v->name_fld.ptr());
  }

  static void on_edit(GtkTreeView *treeview,
		      GtkTreePath *path,
		      GtkTreeViewColumn *col,
		      FeedSearch *v) {
    Ctx &ctx(v->ctx);
    auto feed_rec(get_sel_rec<Feed>(GTK_TREE_VIEW(v->list)));
    assert(feed_rec);
    Feed feed(ctx, *feed_rec);
    FeedView *fv(new FeedView(feed));
    push_view(*fv);
  }
  
  static void on_close(gpointer *_, FeedSearch *v) {
    pop_view(*v);
  }
  
  static void init_list(FeedSearch &v) {
    gtk_widget_set_hexpand(v.list, true);
    add_col(GTK_TREE_VIEW(v.list), "Created", COL_FEED_CREATED);
    add_col(GTK_TREE_VIEW(v.list), "Owner", COL_FEED_OWNER);
    add_col(GTK_TREE_VIEW(v.list), "Name", COL_FEED_NAME, true);
    g_signal_connect(v.list, "row-activated", G_CALLBACK(on_edit), &v);
  }
  
  FeedSearch::FeedSearch(Ctx &ctx):
    View(ctx, "Feed Search"),
    feeds(gtk_list_store_new(4, G_TYPE_POINTER,
			     G_TYPE_STRING, G_TYPE_STRING, G_TYPE_STRING)),
    name(gtk_entry_new()),
    active(gtk_check_button_new_with_label("Active")),
    find(gtk_button_new_with_mnemonic("_Find Feeds")),
    list(gtk_tree_view_new_with_model(GTK_TREE_MODEL(feeds))),
    close(gtk_button_new_with_mnemonic("_Close Search")),
    peer_fld(ctx)
  { }

  void FeedSearch::init() {
    GtkWidget *lbl;

    GtkWidget *frm = gtk_box_new(GTK_ORIENTATION_VERTICAL, 5);
    gtk_box_pack_start(GTK_BOX(panel), frm, false, false, 0);

    lbl = gtk_label_new("Name");
    gtk_widget_set_halign(lbl, GTK_ALIGN_START);
    gtk_container_add(GTK_CONTAINER(frm), lbl);
    GtkWidget *name_box(gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 5));
    gtk_container_add(GTK_CONTAINER(frm), name_box);
    gtk_widget_set_hexpand(name, true);
    gtk_container_add(GTK_CONTAINER(name_box), name);
    gtk_container_add(GTK_CONTAINER(name_box), active);
    gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(active), true);

    lbl = gtk_label_new("Peer");
    gtk_widget_set_halign(lbl, GTK_ALIGN_START);
    gtk_container_add(GTK_CONTAINER(frm), lbl);
    gtk_container_add(GTK_CONTAINER(frm), peer_fld.ptr());
    
    gtk_widget_set_halign(find, GTK_ALIGN_END);
    gtk_widget_set_margin_top(find, 5);
    g_signal_connect(find, "clicked", G_CALLBACK(on_find), this);
    gtk_container_add(GTK_CONTAINER(frm), find);

    init_list(*this);
    gtk_box_pack_start(GTK_BOX(panel), list, true, true, 0);
    lbl = gtk_label_new("Press Return or double-click to edit feed");
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
