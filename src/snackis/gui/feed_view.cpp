#include "snackis/ctx.hpp"
#include "snackis/gui/gui.hpp"
#include "snackis/gui/feed_view.hpp"

namespace snackis {
namespace gui {
  enum PeerCol {COL_PEER_PTR=0, COL_PEER_NAME};
  
  static void on_cancel(gpointer *_, FeedView *frm) {
    log(frm->ctx, "Cancelled feed changes");
    frm->pop_view();
  }

  static void on_save(gpointer *_, FeedView *frm) {
    Ctx &ctx(frm->ctx);
    db::Trans trans(ctx);
    db::commit(trans);
    frm->pop_view();
  }

  void init_peers(FeedView &frm) {
    Ctx &ctx(frm.ctx);

    gtk_widget_set_hexpand(frm.peer_list, true);
    auto rend(gtk_cell_renderer_text_new());
    auto name_col(gtk_tree_view_column_new_with_attributes("Peers",
							   rend,
							   "text", COL_PEER_NAME,
							   nullptr));
    gtk_tree_view_append_column(GTK_TREE_VIEW(frm.peer_list), name_col);    
    
    for(const auto &peer_rec: ctx.db.peers.recs) {
      Peer peer(ctx.db.peers, peer_rec);
      
      GtkTreeIter iter;
      gtk_list_store_append(frm.peers, &iter);
      gtk_list_store_set(frm.peers, &iter,
			 COL_PEER_PTR, &peer_rec,
			 COL_PEER_NAME, peer.name.c_str(),
			 -1);
    }
    
    rend = gtk_cell_renderer_text_new();
    gtk_cell_layout_pack_start(GTK_CELL_LAYOUT(frm.peer), rend, true);
    gtk_cell_layout_set_attributes(GTK_CELL_LAYOUT(frm.peer),
				   rend,
                                   "text", COL_PEER_NAME,
				   nullptr);
    gtk_widget_set_hexpand(frm.peer, true);
    gtk_combo_box_set_active(GTK_COMBO_BOX(frm.peer), 0);
  }
  
  FeedView::FeedView(Ctx &ctx):
    View(ctx, "Feed"),
    peers(gtk_list_store_new(2, G_TYPE_POINTER, G_TYPE_STRING)),
    feed_peers(gtk_list_store_new(2, G_TYPE_POINTER, G_TYPE_STRING)),
    name(gtk_entry_new()),
    peer_list(gtk_tree_view_new_with_model(GTK_TREE_MODEL(feed_peers))),
    peer(gtk_combo_box_new_with_model(GTK_TREE_MODEL(peers))),
    cancel(gtk_button_new_with_mnemonic("_Cancel Changes")),
    save(gtk_button_new_with_mnemonic("_Save Changes")) {
    GtkWidget *lbl;

    GtkWidget *frm = gtk_box_new(GTK_ORIENTATION_VERTICAL, 5);
    gtk_box_pack_start(GTK_BOX(panel), frm, true, true, 0);
    
    lbl = gtk_label_new("Name");
    gtk_widget_set_halign(lbl, GTK_ALIGN_START);
    gtk_container_add(GTK_CONTAINER(frm), lbl);
    gtk_widget_set_hexpand(name, true);
    gtk_container_add(GTK_CONTAINER(frm), name);

    init_peers(*this);
    gtk_container_add(GTK_CONTAINER(frm), peer_list);    
    gtk_container_add(GTK_CONTAINER(frm), peer);
    
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