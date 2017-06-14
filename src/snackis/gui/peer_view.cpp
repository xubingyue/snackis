#include "snackis/ctx.hpp"
#include "snackis/gui/peer_view.hpp"

namespace snackis {
namespace gui {
  static void on_cancel(gpointer *_, PeerView *pev) {
    log(pev->ctx, "Cancelled peer changes");
    pev->pop_view();
  }

  static void on_save(gpointer *_, PeerView *pev) {
    TRACE("PeerView save");
    Ctx &ctx(pev->ctx);
    db::Trans trans(ctx);
    pev->peer.name = gtk_entry_get_text(GTK_ENTRY(pev->name));
    pev->peer.email = gtk_entry_get_text(GTK_ENTRY(pev->email));    
    update(ctx.db.peers, pev->peer);
    db::commit(trans);
    log(ctx, "Saved peer changes");
    pev->pop_view();
  }
  
  PeerView::PeerView(const Peer &peer):
    View(peer.ctx, fmt("Peer %0", peer.name)),
    peer(peer),
    name(gtk_entry_new()),
    email(gtk_entry_new()) {
    GtkWidget *lbl;

    GtkWidget *frm = gtk_box_new(GTK_ORIENTATION_VERTICAL, 5);
    gtk_box_pack_start(GTK_BOX(panel), frm, true, true, 0);

    lbl = gtk_label_new("Name");
    gtk_widget_set_halign(lbl, GTK_ALIGN_START);
    gtk_container_add(GTK_CONTAINER(frm), lbl);
    gtk_widget_set_hexpand(name, true);
    gtk_entry_set_text(GTK_ENTRY(name), peer.name.c_str());
    gtk_container_add(GTK_CONTAINER(frm), name);
    
    lbl = gtk_label_new("Email");
    gtk_widget_set_halign(lbl, GTK_ALIGN_START);  
    gtk_container_add(GTK_CONTAINER(frm), lbl);
    gtk_widget_set_hexpand(email, true);
    gtk_entry_set_text(GTK_ENTRY(email), peer.email.c_str());
    gtk_container_add(GTK_CONTAINER(frm), email);

    GtkWidget *btns = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 5);
    gtk_widget_set_halign(btns, GTK_ALIGN_END);
    gtk_widget_set_valign(btns, GTK_ALIGN_END);
    gtk_container_add(GTK_CONTAINER(panel), btns);
        
    cancel = gtk_button_new_with_mnemonic("_Cancel Changes");
    g_signal_connect(cancel, "clicked", G_CALLBACK(on_cancel), this);
    gtk_container_add(GTK_CONTAINER(btns), cancel);
    
    save = gtk_button_new_with_mnemonic("_Save Changes");
    g_signal_connect(save, "clicked", G_CALLBACK(on_save), this);
    gtk_container_add(GTK_CONTAINER(btns), save);
  }
  
  void PeerView::focus() {
    gtk_widget_grab_focus(name);
  }
}}
