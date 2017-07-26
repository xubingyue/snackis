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
  
  FeedHistory::FeedHistory(Ctx &ctx):
    ctx(ctx),
    store(gtk_tree_store_new(3, G_TYPE_POINTER,
			     G_TYPE_STRING,
			     G_TYPE_STRING)),
    box(gtk_box_new(GTK_ORIENTATION_VERTICAL, 5)),
    lst(new_tree_view(GTK_TREE_MODEL(store)))
  {
    
    add_col(GTK_TREE_VIEW(lst), "", COL_BY);
    add_col(GTK_TREE_VIEW(lst), "", COL_BODY, true);
    gtk_widget_set_hexpand(lst, true);
    g_signal_connect(lst, "row-activated", G_CALLBACK(on_activate), this);
    gtk_container_add(GTK_CONTAINER(box), gtk_widget_get_parent(lst));

    gtk_container_add(GTK_CONTAINER(box),
		      gtk_label_new("Press Return or double-click to view Post"));

  }

  GtkWidget *FeedHistory::ptr() { return box; }

  void clear(FeedHistory &w) {
    gtk_tree_store_clear(w.store);
  }

  static void add_post(FeedHistory &w,
		       const Feed &fd,
		       const db::Rec<Post> *rec,
		       Time start,
		       GtkTreeIter *parent) {
    Post ps(w.ctx, *rec);
    Peer pr(get_peer_id(w.ctx, ps.owner_id));
    
    GtkTreeIter it;
    gtk_tree_store_append(w.store, &it, parent);
    const str by(fmt("%0\n%1",
		     pr.name.c_str(),
		     fmt(ps.created_at, "%a %b %d, %H:%M").c_str()));
    
    gtk_tree_store_set(w.store, &it,
		       COL_PTR, rec,
		       COL_BY, by.c_str(),
		       COL_BODY, ps.body.c_str(),
		       -1);

    if (ps.id != fd.id) {
      auto ps_fd(find_feed_id(w.ctx, ps.id));
      if (ps_fd) {
	for (auto r: last_posts(*ps_fd, start, FEED_HISTORY_MAX)) {
	  add_post(w, *ps_fd, r, start, &it);
	}
      }
    }
  }
  
  void load(FeedHistory &w, const Feed &fd, Time start) {
    auto pst(db::find(w.ctx.db.posts, fd.id));
    if (pst) { add_post(w, fd, pst, start, nullptr); }

    for (auto r: last_posts(fd, start, FEED_HISTORY_MAX)) {
      add_post(w, fd, r, start, nullptr);
    }
  }

  size_t post_count(FeedHistory &w) {
    return gtk_tree_model_iter_n_children(GTK_TREE_MODEL(w.store), nullptr);
  }
}}
