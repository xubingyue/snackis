#include "snackis/ctx.hpp"
#include "snackis/gui/peer_view.hpp"

namespace snackis {
namespace gui {  
  PeerView::PeerView(const Peer &peer):
    RecView<Peer>("Peer", peer),
    name(gtk_entry_new()),
    email(gtk_entry_new()) {
    GtkWidget *lbl;

    lbl = gtk_label_new("Name");
    gtk_widget_set_halign(lbl, GTK_ALIGN_START);
    gtk_container_add(GTK_CONTAINER(fields), lbl);
    gtk_widget_set_hexpand(name, true);
    gtk_entry_set_text(GTK_ENTRY(name), peer.name.c_str());
    gtk_container_add(GTK_CONTAINER(fields), name);
    
    lbl = gtk_label_new("Email");
    gtk_widget_set_halign(lbl, GTK_ALIGN_START);  
    gtk_container_add(GTK_CONTAINER(fields), lbl);
    gtk_widget_set_hexpand(email, true);
    gtk_entry_set_text(GTK_ENTRY(email), peer.email.c_str());
    gtk_container_add(GTK_CONTAINER(fields), email);

    focused = name;
  }

  void PeerView::on_save() {
    TRACE("Saving peer");
    rec.name = gtk_entry_get_text(GTK_ENTRY(name));
    rec.email = gtk_entry_get_text(GTK_ENTRY(email));    
    update(ctx.db.peers, rec);
  }
}}
