#include "snackis/peer.hpp"
#include "snackis/post.hpp"
#include "snackis/core/error.hpp"
#include "snackis/gui/feed_history.hpp"
#include "snackis/gui/gui.hpp"
#include "snackis/gui/post_view.hpp"

namespace snackis {
namespace gui {
  enum PostCol {COL_PTR=0, COL_BY, COL_BODY};

  static void on_activate(GtkTreeView *treeview,
			  GtkTreePath *path,
			  GtkTreeViewColumn *col,
			  FeedHistory *w) {
    auto ps_rec(get_sel_rec<Post>(GTK_TREE_VIEW(w->lst)));
    CHECK(ps_rec, _);
    Post ps(w->ctx, *ps_rec);
    push_view(new PostView(ps));
  }
  
  FeedHistory::FeedHistory(Ctx &ctx, const str &lbl):
    ctx(ctx),
    store(gtk_list_store_new(3, G_TYPE_POINTER,
			     G_TYPE_STRING,
			     G_TYPE_STRING)),
    box(gtk_box_new(GTK_ORIENTATION_VERTICAL, 5)),
    lst(new_tree_view(GTK_TREE_MODEL(store)))
  {
    
    add_col(GTK_TREE_VIEW(lst), lbl, COL_BY);
    add_col(GTK_TREE_VIEW(lst), "", COL_BODY, true);
    gtk_widget_set_hexpand(lst, true);
    g_signal_connect(lst, "row-activated", G_CALLBACK(on_activate), this);
    gtk_container_add(GTK_CONTAINER(box), gtk_widget_get_parent(lst));

    gtk_container_add(GTK_CONTAINER(box),
		      gtk_label_new("Press Return or double-click to edit Post"));

  }

  GtkWidget *FeedHistory::ptr() { return box; }

  void clear(FeedHistory &w) {
    gtk_list_store_clear(w.store);
  }

  void load(FeedHistory &w, const Feed &fd, Time start) {
    for (auto rec: last_posts(fd, start, 30)) {
      Post ps(w.ctx, *rec);
      Peer pr(get_peer_id(w.ctx, ps.owner_id));
      
      GtkTreeIter it;
      gtk_list_store_append(w.store, &it);
      const str by(fmt("%0\n%1",
		       pr.name.c_str(),
		       fmt(ps.created_at, "%a %b %d, %H:%M").c_str()));
      
      gtk_list_store_set(w.store, &it,
			 COL_PTR, rec,
			 COL_BY, by.c_str(),
			 COL_BODY, ps.body.c_str(),
			 -1);
    }
  }
}}
