#include "snackis/ctx.hpp"
#include "snackis/gui/peer_view.hpp"

namespace snackis {
namespace gui {  
  PeerView::PeerView(const Peer &peer):
    RecView<Peer>("Peer", peer),
    name_fld(gtk_entry_new()),
    email_fld(gtk_entry_new()) {
    GtkWidget *lbl;

    lbl = gtk_label_new("Name");
    gtk_widget_set_halign(lbl, GTK_ALIGN_START);
    gtk_container_add(GTK_CONTAINER(fields), lbl);
    gtk_widget_set_hexpand(name_fld, true);
    gtk_entry_set_text(GTK_ENTRY(name_fld), peer.name.c_str());
    gtk_container_add(GTK_CONTAINER(fields), name_fld);
    
    lbl = gtk_label_new("Email");
    gtk_widget_set_halign(lbl, GTK_ALIGN_START);  
    gtk_container_add(GTK_CONTAINER(fields), lbl);
    gtk_widget_set_hexpand(email_fld, true);
    gtk_entry_set_text(GTK_ENTRY(email_fld), peer.email.c_str());
    gtk_container_add(GTK_CONTAINER(fields), email_fld);

    focused = name_fld;
  }

  void PeerView::on_save() {
    rec.name = gtk_entry_get_text(GTK_ENTRY(name_fld));
    rec.email = gtk_entry_get_text(GTK_ENTRY(email_fld));    
    update(ctx.db.peers, rec);
  }
}}
