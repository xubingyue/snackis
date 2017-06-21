#include "snackis/ctx.hpp"
#include "snackis/gui/peer_view.hpp"

namespace snackis {
namespace gui {
  static void on_cancel(gpointer *_, PeerView *v) {
    log(v->ctx, "Cancelled peer");
    pop_view(*v);
  }

  static void on_save(gpointer *_, PeerView *v) {
    TRACE("Saving peer view");
    Ctx &ctx(v->ctx);
    db::Trans trans(ctx);
    v->peer.name = gtk_entry_get_text(GTK_ENTRY(v->name));
    v->peer.email = gtk_entry_get_text(GTK_ENTRY(v->email));    
    update(ctx.db.peers, v->peer);
    db::commit(trans);
    log(ctx, "Saved peer");
    pop_view(*v);
  }
  
  PeerView::PeerView(const Peer &peer):
    View(peer.ctx, "Peer"),
    peer(peer),
    name(gtk_entry_new()),
    email(gtk_entry_new()),
    save(gtk_button_new_with_mnemonic("_Save Peer")),
    cancel(gtk_button_new_with_mnemonic("_Cancel")) {
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
        
    g_signal_connect(save, "clicked", G_CALLBACK(on_save), this);
    gtk_container_add(GTK_CONTAINER(btns), save);

    g_signal_connect(cancel, "clicked", G_CALLBACK(on_cancel), this);
    gtk_container_add(GTK_CONTAINER(btns), cancel);
  }
  
  void PeerView::focus() {
    gtk_widget_grab_focus(name);
  }
}}
