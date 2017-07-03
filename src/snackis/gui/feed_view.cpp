#include <cassert>
#include "snackis/ctx.hpp"
#include "snackis/gui/gui.hpp"
#include "snackis/gui/feed_view.hpp"
#include "snackis/gui/peer_search.hpp"
#include "snackis/gui/post_search.hpp"
#include "snackis/gui/post_view.hpp"

namespace snackis {
namespace gui {
  enum PeerCol {COL_PEER_PTR=0, COL_PEER_ID, COL_PEER_NAME};
  enum PostCol {COL_POST_PTR=0, COL_POST_BY, COL_POST_BODY};
  
  static void on_add_peer(gpointer *_, FeedView *v) {
    Ctx &ctx(v->ctx);

    PeerSearch *ps = new PeerSearch(v->ctx);    
    ps->on_activate = [v, ps, &ctx](auto &rec) {
      Peer peer(ctx, rec);    

      if (v->rec.peer_ids.insert(peer.id).second) {
	GtkTreeIter iter;    
	gtk_list_store_append(v->peer_store, &iter);
	gtk_list_store_set(v->peer_store, &iter,
			   COL_PEER_PTR, &rec,
			   COL_PEER_ID, id_str(peer).c_str(),
			   COL_PEER_NAME, peer.name.c_str(),
			   -1);
	auto sel(gtk_tree_view_get_selection(GTK_TREE_VIEW(v->peer_lst)));
	gtk_tree_selection_select_iter(sel, &iter);
      }
      
      pop_view(*ps);
      gtk_widget_grab_focus(v->add_peer_btn);
    };
    
    push_view(*ps);
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
      gtk_list_store_remove(v->peer_store, &it);
    }
  }
  
  static void load_peers(FeedView &v) {
    Ctx &ctx(v.ctx);
    
    for (const auto &peer_id: v.rec.peer_ids) {
      db::Rec<Peer> key;
      db::set(key, ctx.db.peer_id, peer_id);
      const db::Rec<Peer> *rec(db::find(ctx.db.peers, key));
      
      if (rec) {
	Peer peer(ctx, *rec);
	GtkTreeIter iter;
	gtk_list_store_append(v.peer_store, &iter);
	gtk_list_store_set(v.peer_store, &iter,
			   COL_PEER_PTR, rec,
			   COL_PEER_ID, id_str(peer).c_str(),
			   COL_PEER_NAME, peer.name.c_str(),
			   -1);
      }
    }
  }
  
  static void init_peers(FeedView &v) {
    GtkWidget *peer_box(gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 5));
    gtk_container_add(GTK_CONTAINER(v.fields), peer_box);
    gtk_widget_set_margin_top(v.peer_lst, 5);
    gtk_widget_set_hexpand(v.peer_lst, true);
    gtk_widget_set_vexpand(v.peer_lst, true);
    add_col(GTK_TREE_VIEW(v.peer_lst), "Peers", COL_PEER_ID);
    add_col(GTK_TREE_VIEW(v.peer_lst), "", COL_PEER_NAME);
    g_signal_connect(v.peer_lst, "row-activated", G_CALLBACK(on_remove_peer), &v);    
    gtk_container_add(GTK_CONTAINER(peer_box), v.peer_lst);
    gtk_widget_set_valign(v.add_peer_btn, GTK_ALIGN_END);
    g_signal_connect(v.add_peer_btn, "clicked", G_CALLBACK(on_add_peer), &v);
    gtk_container_add(GTK_CONTAINER(peer_box), v.add_peer_btn);
    gtk_container_add(GTK_CONTAINER(v.fields),
		      gtk_label_new("Press Return or double-click to remove peer"));
    load_peers(v);
  }

  static void on_find_posts(gpointer *_, FeedView *v) {
    PostSearch *ps = new PostSearch(v->ctx);
    ps->feed.emplace(v->rec);
    push_view(*ps);
  }

  static void on_post(gpointer *_, FeedView *v) {
    {
      db::Trans trans(v->ctx);
      v->save();
      db::commit(trans);
    }
    
    Post post(v->ctx);
    post.feed_id = v->rec.id;
    PostView *pv = new PostView(post);
    push_view(*pv);
  }
  
  FeedView::FeedView(const Feed &feed):
    RecView("Feed", feed),
    peer_store(gtk_list_store_new(4, G_TYPE_POINTER,
				  G_TYPE_STRING, G_TYPE_STRING, G_TYPE_STRING)),
    find_posts_btn(gtk_button_new_with_mnemonic("_Find Posts")),
    post_btn(gtk_button_new_with_mnemonic("New _Post")),
    name_fld(gtk_entry_new()),
    active_fld(gtk_check_button_new_with_label("Active")),
    info_fld(new_text_view()),
    peer_lst(gtk_tree_view_new_with_model(GTK_TREE_MODEL(peer_store))),
    add_peer_btn(gtk_button_new_with_mnemonic("_Add Peer"))
  { }

  void FeedView::init() {
    GtkWidget *lbl;

    GtkWidget *btns = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 5);
    gtk_widget_set_margin_bottom(btns, 5);
    gtk_container_add(GTK_CONTAINER(fields), btns);
    g_signal_connect(find_posts_btn, "clicked", G_CALLBACK(on_find_posts), this);
    gtk_container_add(GTK_CONTAINER(btns), find_posts_btn);
    g_signal_connect(post_btn, "clicked", G_CALLBACK(on_post), this);
    gtk_container_add(GTK_CONTAINER(btns), post_btn);

    lbl = gtk_label_new("Name");
    gtk_widget_set_halign(lbl, GTK_ALIGN_START);
    gtk_container_add(GTK_CONTAINER(fields), lbl);
    GtkWidget *name_box = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 5);
    gtk_container_add(GTK_CONTAINER(fields), name_box);
    gtk_widget_set_hexpand(name_fld, true);
    gtk_container_add(GTK_CONTAINER(name_box), name_fld);    
    gtk_entry_set_text(GTK_ENTRY(name_fld), rec.name.c_str());
    gtk_container_add(GTK_CONTAINER(name_box), active_fld);
    gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(active_fld), rec.active);

    lbl = gtk_label_new("Info");
    gtk_widget_set_halign(lbl, GTK_ALIGN_START);
    gtk_container_add(GTK_CONTAINER(fields), lbl);
    gtk_container_add(GTK_CONTAINER(fields), gtk_widget_get_parent(info_fld));
    set_str(GTK_TEXT_VIEW(info_fld), rec.info);
    
    init_peers(*this);
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
    return true;
  }

}}
