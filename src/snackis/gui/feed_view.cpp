#include "snackis/ctx.hpp"
#include "snackis/gui/gui.hpp"
#include "snackis/gui/feed_view.hpp"

namespace snackis {
namespace gui {
  enum PeerCol {COL_PEER_PTR=0, COL_PEER_NAME};
  
  static void on_cancel(gpointer *_, FeedView *v) {
    log(v->ctx, "Cancelled feed changes");
    pop_view(*v);
  }

  static void on_save(gpointer *_, FeedView *v) {
    Ctx &ctx(v->ctx);
    db::Trans trans(ctx);
    db::commit(trans);
    pop_view(*v);
  }

  static db::Rec<Peer> &get_sel_peer(FeedView &v) {
    GtkTreeIter iter;
    gtk_combo_box_get_active_iter(GTK_COMBO_BOX(v.peer), &iter);
    db::Rec<Peer> *rec;
    gtk_tree_model_get(GTK_TREE_MODEL(v.peers), &iter, COL_PEER_PTR, &rec, -1);
    return *rec;
  }

  static void on_peer_sel(gpointer *_, FeedView *v) {
    Ctx &ctx(v->ctx);
    db::Rec<Peer> &rec(get_sel_peer(*v));
    auto id(*db::get(rec, ctx.db.peer_id));
    bool exists = v->feed.peer_ids.find(id) != v->feed.peer_ids.end();
    gtk_widget_set_sensitive(v->add_peer, !exists);
    gtk_widget_set_sensitive(v->remove_peer, exists);
  }
  
  static void on_add_peer(gpointer *_, FeedView *v) {
    Ctx &ctx(v->ctx);
    db::Rec<Peer> &rec(get_sel_peer(*v));
    GtkTreeIter iter;    
    gtk_list_store_append(v->feed_peers, &iter);
    gtk_list_store_set(v->feed_peers, &iter,
		       COL_PEER_PTR, &rec,
		       COL_PEER_NAME, db::get(rec, ctx.db.peer_name)->c_str(),
		       -1);
    v->feed.peer_ids.insert(*db::get(rec, ctx.db.peer_id));
    gtk_widget_set_sensitive(v->add_peer, false);
    gtk_widget_set_sensitive(v->remove_peer, true);    
  }

  static void on_remove_peer(gpointer *_, FeedView *v) {
    Ctx &ctx(v->ctx);
    db::Rec<Peer> &rec(get_sel_peer(*v));    
    GtkTreeIter iter;    
    gtk_tree_model_get_iter_first(GTK_TREE_MODEL(v->feed_peers), &iter);
    
    do {
      db::Rec<Peer> *iter_rec;
      gtk_tree_model_get(GTK_TREE_MODEL(v->feed_peers),
			 &iter,
			 COL_PEER_PTR, &iter_rec,
			 -1);
      if (iter_rec == &rec) {
	gtk_list_store_remove(v->feed_peers, &iter);
	break;
      }
    } while (gtk_tree_model_iter_next(GTK_TREE_MODEL(v->feed_peers), &iter));
    
    v->feed.peer_ids.erase(*db::get(rec, ctx.db.peer_id));
    gtk_widget_set_sensitive(v->add_peer, true);
    gtk_widget_set_sensitive(v->remove_peer, false);    
  }
  
  void init_peers(FeedView &v) {
    Ctx &ctx(v.ctx);

    gtk_widget_set_hexpand(v.peer_list, true);
    auto rend(gtk_cell_renderer_text_new());
    auto name_col(gtk_tree_view_column_new_with_attributes("Peers",
							   rend,
							   "text", COL_PEER_NAME,
							   nullptr));
    gtk_tree_view_append_column(GTK_TREE_VIEW(v.peer_list), name_col);    
    
    for(const auto &peer_rec: ctx.db.peers.recs) {
      Peer peer(ctx, peer_rec);
      
      GtkTreeIter iter;
      gtk_list_store_append(v.peers, &iter);
      gtk_list_store_set(v.peers, &iter,
			 COL_PEER_PTR, &peer_rec,
			 COL_PEER_NAME, peer.name.c_str(),
			 -1);
    }

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
    
    g_signal_connect(v.remove_peer, "clicked", G_CALLBACK(on_remove_peer), &v);
    gtk_container_add(GTK_CONTAINER(v.peer_input), v.remove_peer);
  }
  
  FeedView::FeedView(Ctx &ctx):
    View(ctx, "Feed"),
    feed(ctx),
    peers(gtk_list_store_new(2, G_TYPE_POINTER, G_TYPE_STRING)),
    feed_peers(gtk_list_store_new(2, G_TYPE_POINTER, G_TYPE_STRING)),
    name(gtk_entry_new()),
    peer_list(gtk_tree_view_new_with_model(GTK_TREE_MODEL(feed_peers))),
    peer_input(gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 5)),
    peer(gtk_combo_box_new_with_model(GTK_TREE_MODEL(peers))),
    add_peer(gtk_button_new_with_mnemonic("Add")),
    remove_peer(gtk_button_new_with_mnemonic("Remove")),
    cancel(gtk_button_new_with_mnemonic("_Cancel Changes")),
    save(gtk_button_new_with_mnemonic("_Save Changes")) {
    GtkWidget *lbl;

    GtkWidget *v = gtk_box_new(GTK_ORIENTATION_VERTICAL, 5);
    gtk_box_pack_start(GTK_BOX(panel), v, true, true, 0);
    
    lbl = gtk_label_new("Name");
    gtk_widget_set_halign(lbl, GTK_ALIGN_START);
    gtk_container_add(GTK_CONTAINER(v), lbl);
    gtk_widget_set_hexpand(name, true);
    gtk_container_add(GTK_CONTAINER(v), name);

    init_peers(*this);
    gtk_container_add(GTK_CONTAINER(v), peer_list);    
    gtk_container_add(GTK_CONTAINER(v), peer_input);
    
    GtkWidget *btns = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 5);
    gtk_widget_set_halign(btns, GTK_ALIGN_END);
    gtk_widget_set_valign(btns, GTK_ALIGN_END);
    gtk_container_add(GTK_CONTAINER(panel), btns);
        
    g_signal_connect(cancel, "clicked", G_CALLBACK(on_cancel), this);
    gtk_container_add(GTK_CONTAINER(btns), cancel);
    
    g_signal_connect(save, "clicked", G_CALLBACK(on_save), this);
    gtk_container_add(GTK_CONTAINER(btns), save);
  }
  
  void FeedView::focus() {
    gtk_widget_grab_focus(name);
  }
}}
