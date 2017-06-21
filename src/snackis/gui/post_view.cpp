#include "snackis/ctx.hpp"
#include "snackis/gui/gui.hpp"
#include "snackis/gui/post_view.hpp"

namespace snackis {
namespace gui {
  enum FeedCol {COL_FEED_PTR=0, COL_FEED_NAME};
    
  static void on_cancel(gpointer *_, PostView *v) {
    log(v->ctx, "Cancelled post");
    pop_view(*v);
  }

  static void on_save(gpointer *_, PostView *v) {
    Ctx &ctx(v->ctx);
    db::Trans trans(ctx);
    db::commit(trans);
    log(v->ctx, "Saved post");
    pop_view(*v);
  }

  void init_feeds(PostView &v) {
    Ctx &ctx(v.ctx);
    
    for(const auto &f: ctx.db.feeds.recs) {
      GtkTreeIter iter;
      gtk_list_store_append(v.feeds, &iter);
      gtk_list_store_set(v.feeds, &iter,
			 COL_FEED_PTR, &f,
			 COL_FEED_NAME, db::get(f, ctx.db.feed_name)->c_str(),
			 -1);
    }
    
    auto col(gtk_cell_renderer_text_new());
    gtk_cell_layout_pack_start(GTK_CELL_LAYOUT(v.feed), col, true);
    gtk_cell_layout_set_attributes(GTK_CELL_LAYOUT(v.feed),
				   col,
                                   "text", COL_FEED_NAME,
				   nullptr);
    gtk_combo_box_set_active(GTK_COMBO_BOX(v.feed), 0);
  }
  
  PostView::PostView(const Post &post):
    View(post.ctx, "Post"),
    post(post),
    feeds(gtk_list_store_new(2, G_TYPE_POINTER, G_TYPE_STRING)),
    feed(gtk_combo_box_new_with_model(GTK_TREE_MODEL(feeds))),
    save(gtk_button_new_with_mnemonic("_Save")),
    cancel(gtk_button_new_with_mnemonic("_Cancel")) {
    GtkWidget *lbl;

    GtkWidget *frm = gtk_box_new(GTK_ORIENTATION_VERTICAL, 5);
    gtk_box_pack_start(GTK_BOX(panel), frm, true, true, 0);
    
    lbl = gtk_label_new("Feed");
    gtk_widget_set_halign(lbl, GTK_ALIGN_START);
    gtk_container_add(GTK_CONTAINER(frm), lbl);
    gtk_widget_set_hexpand(feed, true);
    gtk_container_add(GTK_CONTAINER(frm), feed);
    init_feeds(*this);
    
    GtkWidget *btns = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 5);
    gtk_widget_set_halign(btns, GTK_ALIGN_END);
    gtk_widget_set_valign(btns, GTK_ALIGN_END);
    gtk_container_add(GTK_CONTAINER(panel), btns);
        
    g_signal_connect(save, "clicked", G_CALLBACK(on_save), this);
    gtk_container_add(GTK_CONTAINER(btns), save);

    g_signal_connect(cancel, "clicked", G_CALLBACK(on_cancel), this);
    gtk_container_add(GTK_CONTAINER(btns), cancel);
  }
  
  void PostView::focus() {
    gtk_widget_grab_focus(feed);
  }
}}
