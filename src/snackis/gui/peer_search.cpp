#include <cassert>
#include "snackis/ctx.hpp"
#include "snackis/gui/gui.hpp"
#include "snackis/gui/peer_search.hpp"
#include "snackis/gui/peer_view.hpp"

namespace snackis {
namespace gui {
  enum PeerCol {COL_PTR=0, COL_ID, COL_NAME, COL_EMAIL};

  static void edit(Ctx &ctx, const db::Rec<Peer> &rec) {
    Peer peer(ctx, rec);
    PeerView *fv(new PeerView(peer));
    push_view(*fv);
  }  

  PeerSearch::PeerSearch(Ctx &ctx):
    SearchView<Peer>(ctx, "Peer",
		     gtk_list_store_new(4,
					G_TYPE_POINTER,
					G_TYPE_STRING,
					G_TYPE_STRING,
					G_TYPE_STRING),
		     [&ctx](auto &rec) { edit(ctx, rec); }),
    id_fld(new_id_field()),
    name_fld(gtk_entry_new()),
    email_fld(gtk_entry_new())
  { }

  void PeerSearch::init() {
    SearchView<Peer>::init();
    GtkWidget *lbl;

    auto name_box(gtk_grid_new());
    gtk_grid_set_row_spacing(GTK_GRID(name_box), 5);
    gtk_grid_set_column_spacing(GTK_GRID(name_box), 5);
    gtk_container_add(GTK_CONTAINER(fields), name_box);

    lbl = gtk_label_new("Id");
    gtk_widget_set_halign(lbl, GTK_ALIGN_START);
    gtk_grid_attach(GTK_GRID(name_box), lbl, 0, 0, 1, 1);
    gtk_grid_attach(GTK_GRID(name_box), id_fld, 0, 1, 1, 1);

    lbl = gtk_label_new("Name");
    gtk_widget_set_halign(lbl, GTK_ALIGN_START);
    gtk_grid_attach(GTK_GRID(name_box), lbl, 1, 0, 1, 1);
    gtk_widget_set_hexpand(name_fld, true);
    gtk_grid_attach(GTK_GRID(name_box), name_fld, 1, 1, 1, 1);

    lbl = gtk_label_new("Email");
    gtk_widget_set_halign(lbl, GTK_ALIGN_START);
    gtk_container_add(GTK_CONTAINER(fields), lbl);
    gtk_widget_set_hexpand(email_fld, true);
    gtk_container_add(GTK_CONTAINER(fields), email_fld);

    add_col(GTK_TREE_VIEW(list), "Id", COL_ID);
    add_col(GTK_TREE_VIEW(list), "Name", COL_NAME);
    add_col(GTK_TREE_VIEW(list), "Email", COL_EMAIL);

    focused = id_fld;
  }
  
  void PeerSearch::find() {
    size_t cnt(0);    
    str id_sel(trim(gtk_entry_get_text(GTK_ENTRY(id_fld))));
    str name_sel(trim(gtk_entry_get_text(GTK_ENTRY(name_fld))));
    str email_sel(trim(gtk_entry_get_text(GTK_ENTRY(email_fld))));
    
    for (auto key = ctx.db.peers_sort.recs.begin();
	 key != ctx.db.peers_sort.recs.end();
	 key++) {
      auto &rec(db::get(ctx.db.peers, *key));
      Peer peer(ctx, rec);

      if (!id_sel.empty() && find_ci(id_str(peer), id_sel) == str::npos) {
	continue;
      }

      if (!name_sel.empty() && find_ci(peer.name, name_sel) == str::npos) {
	continue;
      }

      if (!email_sel.empty() && find_ci(peer.email, email_sel) == str::npos) {
	continue;
      }

      GtkTreeIter iter;
      gtk_list_store_append(store, &iter);
      gtk_list_store_set(store, &iter,
			 COL_PTR, &rec,
			 COL_ID, id_str(peer).c_str(),
			 COL_NAME, peer.name.c_str(),
			 COL_EMAIL, peer.email.c_str(),
			 -1);
      cnt++;
    }

    gtk_widget_grab_focus(cnt ? list : id_fld);
  }
}}
