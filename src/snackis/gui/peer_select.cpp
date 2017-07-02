#include "snackis/gui/peer_search.hpp"
#include "snackis/gui/peer_select.hpp"

namespace snackis {
namespace gui {
  static void on_search(gpointer *_, PeerSelect *v) {
    PeerSearch *ps = new PeerSearch(v->ctx);
    
    ps->on_activate = [v, ps](auto &rec) {
      v->selected = &rec;
      Peer peer(v->ctx, rec);
      gtk_entry_set_text(GTK_ENTRY(v->id_fld), id_str(peer).c_str());
      gtk_entry_set_text(GTK_ENTRY(v->name_fld), peer.name.c_str());
      gtk_widget_set_sensitive(v->clear_btn, true);
      pop_view(*ps);
    };
    
    push_view(*ps);
  }

  static void on_clear(gpointer *_, PeerSelect *v) {
    v->selected = nullptr;
    gtk_entry_set_text(GTK_ENTRY(v->id_fld), "");
    gtk_entry_set_text(GTK_ENTRY(v->name_fld), "");
  }

  PeerSelect::PeerSelect(Ctx &ctx):
    ctx(ctx),
    box(gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 5)),
    id_fld(gtk_entry_new()),
    name_fld(gtk_entry_new()),
    search_btn(gtk_button_new_with_mnemonic("Search")),
    clear_btn(gtk_button_new_with_mnemonic("Clear")),
    selected(nullptr)
  {
    gtk_widget_set_sensitive(id_fld, false);
    gtk_container_add(GTK_CONTAINER(box), id_fld);
    gtk_widget_set_sensitive(name_fld, false);
    gtk_widget_set_hexpand(name_fld, true);
    gtk_container_add(GTK_CONTAINER(box), name_fld);
    g_signal_connect(search_btn, "clicked", G_CALLBACK(on_search), this);
    gtk_container_add(GTK_CONTAINER(box), search_btn);
    gtk_widget_set_sensitive(clear_btn, false);
    g_signal_connect(clear_btn, "clicked", G_CALLBACK(on_clear), this);
    gtk_container_add(GTK_CONTAINER(box), clear_btn);
  }

  GtkWidget *PeerSelect::ptr() { return box; }
}}
