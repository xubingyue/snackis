#include "snackis/ctx.hpp"
#include "snackis/gui/gui.hpp"
#include "snackis/gui/peer_view.hpp"
#include "snackis/gui/post_search.hpp"

namespace snackis {
namespace gui {
  static void on_find_posts(gpointer *_, PeerView *v) {
    PostSearch *ps = new PostSearch(v->ctx);
    select<Peer>(ps->peer_fld, v->rec);
    push_view(*ps);
  }
  
  PeerView::PeerView(const Peer &pr):
    RecView<Peer>("Peer", pr),
    find_posts_btn(gtk_button_new_with_mnemonic("_Find Posts")),
    created_at_fld(gtk_entry_new()),
    changed_at_fld(gtk_entry_new()),
    name_fld(gtk_entry_new()),
    email_fld(gtk_entry_new())
  {
    g_signal_connect(find_posts_btn, "clicked", G_CALLBACK(on_find_posts), this);
    gtk_container_add(GTK_CONTAINER(menu), find_posts_btn);
    
    GtkWidget *frm(gtk_grid_new());
    gtk_grid_set_row_spacing(GTK_GRID(frm), 5);
    gtk_grid_set_column_spacing(GTK_GRID(frm), 5);
    gtk_widget_set_halign(frm, GTK_ALIGN_END);    
    gtk_container_add(GTK_CONTAINER(fields), frm);

    gtk_grid_attach(GTK_GRID(frm), new_label("Created At"), 0, 0, 1, 1);
    gtk_widget_set_sensitive(created_at_fld, false);
    gtk_grid_attach(GTK_GRID(frm), created_at_fld, 0, 1, 1, 1);
    set_str(GTK_ENTRY(created_at_fld), fmt(pr.created_at, "%a %b %d, %H:%M"));

    gtk_grid_attach(GTK_GRID(frm), new_label("Changed At"), 1, 0, 1, 1);    
    gtk_widget_set_sensitive(changed_at_fld, false);
    gtk_grid_attach(GTK_GRID(frm), changed_at_fld, 1, 1, 1, 1);    
    set_str(GTK_ENTRY(changed_at_fld), fmt(pr.changed_at, "%a %b %d, %H:%M"));

    frm = gtk_grid_new();
    gtk_grid_set_row_spacing(GTK_GRID(frm), 5);
    gtk_grid_set_column_spacing(GTK_GRID(frm), 5);
    gtk_container_add(GTK_CONTAINER(fields), frm);
    
    gtk_grid_attach(GTK_GRID(frm), new_label("Name"), 0, 0, 1, 1);
    gtk_widget_set_hexpand(name_fld, true);
    gtk_grid_attach(GTK_GRID(frm), name_fld, 0, 1, 1, 1);
    set_str(GTK_ENTRY(name_fld), pr.name);
    
    gtk_grid_attach(GTK_GRID(frm), new_label("Email"), 1, 0, 1, 1);
    gtk_widget_set_hexpand(email_fld, true);
    gtk_grid_attach(GTK_GRID(frm), email_fld, 1, 1, 1, 1);    
    set_str(GTK_ENTRY(email_fld), pr.email);

    focused = name_fld;
  }

  bool PeerView::save() {
    rec.name = gtk_entry_get_text(GTK_ENTRY(name_fld));
    rec.email = gtk_entry_get_text(GTK_ENTRY(email_fld));    
    db::upsert(ctx.db.peers, rec);
    return true;
  }
}}
