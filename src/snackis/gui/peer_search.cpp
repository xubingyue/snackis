#include <cassert>
#include "snackis/ctx.hpp"
#include "snackis/gui/gui.hpp"
#include "snackis/gui/peer_search.hpp"
#include "snackis/gui/peer_view.hpp"

namespace snackis {
namespace gui {
  enum PeerCol {COL_PTR=0, COL_ID, COL_NAME, COL_TAGS, COL_EMAIL};

  static void edit(Ctx &ctx, const db::Rec<Peer> &rec) {
    push_view(new PeerView(Peer(ctx, rec)));
  }  

  PeerSearch::PeerSearch(Ctx &ctx):
    SearchView<Peer>(ctx, "Peer",
		     gtk_list_store_new(5,
					G_TYPE_POINTER,
					G_TYPE_STRING,
					G_TYPE_STRING,
					G_TYPE_STRING,
					G_TYPE_STRING),
		     [&ctx](auto &rec) { edit(ctx, rec); }),
    id_fld(new_id_field()),
    active_fld(gtk_check_button_new_with_label("Active")),
    tags_fld(gtk_entry_new()),
    text_fld(gtk_entry_new())
  {
    auto box(new_grid());
    gtk_grid_set_column_homogeneous(GTK_GRID(box), true);
    gtk_container_add(GTK_CONTAINER(fields), box);
    int row(0);
    
    gtk_grid_attach(GTK_GRID(box), new_label("Id"), 0, row, 1, 1);
    gtk_grid_attach(GTK_GRID(box), id_fld, 0, row+1, 1, 1);
    gtk_widget_set_halign(active_fld, GTK_ALIGN_END);
    gtk_grid_attach(GTK_GRID(box), active_fld, 1, row+1, 1, 1);
    gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(active_fld), true);
    
    row += 2;
    gtk_grid_attach(GTK_GRID(box), new_label("Tags"), 0, row, 1, 1);
    gtk_widget_set_hexpand(tags_fld, true);
    gtk_grid_attach(GTK_GRID(box), tags_fld, 0, row+1, 1, 1);

    gtk_grid_attach(GTK_GRID(box), new_label("Text"), 1, row, 1, 1);
    gtk_widget_set_hexpand(text_fld, true);
    gtk_grid_attach(GTK_GRID(box), text_fld, 1, row+1, 1, 1);

    add_col(GTK_TREE_VIEW(list), "Id", COL_ID);
    add_col(GTK_TREE_VIEW(list), "Name", COL_NAME);
    add_col(GTK_TREE_VIEW(list), "Tags", COL_TAGS);
    add_col(GTK_TREE_VIEW(list), "Email", COL_EMAIL, true);

    focused = id_fld;
  }

  void PeerSearch::find() {
    size_t cnt(0);    
    str id_sel(trim(gtk_entry_get_text(GTK_ENTRY(id_fld))));
    bool active_sel(gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(active_fld)));
    str tags_str(get_str(GTK_ENTRY(tags_fld)));
    std::set<str> tags_sel(word_set(tags_str));
    str text_sel(trim(gtk_entry_get_text(GTK_ENTRY(text_fld))));
    
    for (auto key = ctx.db.peers_sort.recs.begin();
	 key != ctx.db.peers_sort.recs.end();
	 key++) {
      auto &rec(db::get(ctx.db.peers, *key));
      Peer peer(ctx, rec);

      if (!id_sel.empty() && find_ci(id_str(peer), id_sel) == str::npos) {
	continue;
      }

      if (peer.active != active_sel) { continue; }

      if (!tags_sel.empty()) {
	std::vector<str> diff;
	std::set_difference(tags_sel.begin(), tags_sel.end(),
			    peer.tags.begin(), peer.tags.end(),
			    std::back_inserter(diff));
	if (!diff.empty()) { continue; }
      }
      
      if (!text_sel.empty() &&
	  find_ci(peer.name, text_sel) == str::npos &&
	  find_ci(peer.email, text_sel) == str::npos) {
	continue;
      }

      GtkTreeIter iter;
      gtk_list_store_append(store, &iter);
      gtk_list_store_set(store, &iter,
			 COL_PTR, &rec,
			 COL_ID, id_str(peer).c_str(),
			 COL_NAME, peer.name.c_str(),
			 COL_TAGS,
			 join(peer.tags.begin(), peer.tags.end(), '\n').c_str(),
			 COL_EMAIL, peer.email.c_str(),
			 -1);
      cnt++;
    }

    gtk_widget_grab_focus(cnt ? list : id_fld);
  }
}}
