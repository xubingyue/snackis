#include <cassert>
#include "snackis/ctx.hpp"
#include "snackis/gui/gui.hpp"
#include "snackis/gui/feed_search.hpp"
#include "snackis/gui/feed_view.hpp"

namespace snackis {
namespace gui {
  enum FeedCol {COL_PTR=0, COL_ID, COL_CREATED, COL_OWNER, COL_NAME};

  static void edit(Ctx &ctx, const db::Rec<Feed> &rec) {
    Feed feed(ctx, rec);
    FeedView *fv(new FeedView(feed));
    push_view(*fv);
  }
  
  FeedSearch::FeedSearch(Ctx &ctx):
    SearchView<Feed>(ctx,
		     "Feed",
		     gtk_list_store_new(5,
					G_TYPE_POINTER,
					G_TYPE_STRING,
					G_TYPE_STRING,
					G_TYPE_STRING,
					G_TYPE_STRING),
		     [&ctx](auto &rec) { edit(ctx, rec); }),
    id_fld(new_id_field()),
    name_fld(gtk_entry_new()),
    active_fld(gtk_check_button_new_with_label("Active")),
    peer_fld(ctx)
  {
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
    gtk_grid_attach(GTK_GRID(name_box), active_fld, 2, 1, 1, 1);
    gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(active_fld), true);

    lbl = gtk_label_new("Peer");
    gtk_widget_set_halign(lbl, GTK_ALIGN_START);
    gtk_container_add(GTK_CONTAINER(fields), lbl);
    gtk_container_add(GTK_CONTAINER(fields), peer_fld.ptr());

    add_col(GTK_TREE_VIEW(list), "Id", COL_ID);
    add_col(GTK_TREE_VIEW(list), "Created", COL_CREATED);
    add_col(GTK_TREE_VIEW(list), "Owner", COL_OWNER);
    add_col(GTK_TREE_VIEW(list), "Name", COL_NAME, true);

    focused = id_fld;
  }

  void FeedSearch::find() {
    size_t cnt(0);
    
    str id_sel(trim(gtk_entry_get_text(GTK_ENTRY(id_fld))));
    str name_sel(trim(gtk_entry_get_text(GTK_ENTRY(name_fld))));
    auto &peer_sel(peer_fld.selected);
    bool active_sel(gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(active_fld)));
    
    for (auto key = ctx.db.feeds_sort.recs.begin();
	 key != ctx.db.feeds_sort.recs.end();
	 key++) {
      auto &rec(db::get(ctx.db.feeds, *key));
      Feed feed(ctx, rec);

      if (!feed.visible) { continue; }
      if (feed.active != active_sel) { continue; }

      if (peer_sel) {
	if (feed.owner_id != peer_sel->id &&
	    feed.peer_ids.find(peer_sel->id) == feed.peer_ids.end()) { continue; }
      }      

      if (!id_sel.empty() && find_ci(id_str(feed), id_sel) == str::npos) {
	continue;
      }

      if (!name_sel.empty() && find_ci(feed.name, name_sel) == str::npos) {
	continue;
      }
      
      Peer own(get_peer_id(ctx, feed.owner_id));

      GtkTreeIter iter;
      gtk_list_store_append(store, &iter);
      gtk_list_store_set(store, &iter,
			 COL_PTR, &rec,
			 COL_ID, id_str(feed).c_str(),
			 COL_CREATED,
			 fmt(feed.created_at, "%a %b %d, %H:%M").c_str(),
			 COL_OWNER, own.name.c_str(),
			 COL_NAME, feed.name.c_str(),
			 -1);
      cnt++;
    }

    gtk_widget_grab_focus(cnt ? list : id_fld);
  }
}}
