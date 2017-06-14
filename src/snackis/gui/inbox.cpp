#include "snackis/ctx.hpp"
#include "snackis/gui/gui.hpp"
#include "snackis/gui/inbox.hpp"
#include "snackis/gui/reader.hpp"

namespace snackis {
namespace gui {
  enum Cols { NAME_COL=0, EMAIL_COL, INFO_COL };
  
  static void on_close(gpointer *_, Inbox *ibx) {
    ibx->pop_view();
  }

  static void init_list(Inbox &ibx) {
    GtkCellRenderer *rend;

    rend = gtk_cell_renderer_text_new();
    auto name_col(gtk_tree_view_column_new_with_attributes("From",
							   rend,
							   "text", NAME_COL,
							   nullptr));
    gtk_tree_view_append_column(GTK_TREE_VIEW(ibx.list), name_col);    
  
    rend = gtk_cell_renderer_text_new();
    auto email_col(gtk_tree_view_column_new_with_attributes("Email",
							    rend,
							    "text", EMAIL_COL,
							    nullptr));
    gtk_tree_view_append_column(GTK_TREE_VIEW(ibx.list), email_col);

    rend = gtk_cell_renderer_text_new();
    auto info_col(gtk_tree_view_column_new_with_attributes("Info",
							   rend,
							   "text", INFO_COL,
							   nullptr));
    gtk_tree_view_column_set_expand(GTK_TREE_VIEW_COLUMN(info_col), true);
    gtk_tree_view_append_column(GTK_TREE_VIEW(ibx.list), info_col);

    auto mod(gtk_list_store_new(3, G_TYPE_STRING, G_TYPE_STRING, G_TYPE_STRING));

    for(const auto &msg_rec: ibx.ctx.db.inbox.recs) {
      Msg msg(ibx.ctx.db.inbox, msg_rec);
      GtkTreeIter iter;
      gtk_list_store_append(mod, &iter);

      opt<Peer> peer(find_peer_email(ibx.ctx, msg.from));
      gtk_list_store_set(mod, &iter,
			 NAME_COL, peer ? peer->name.c_str() : msg.peer_name.c_str(),
			 EMAIL_COL, msg.from.c_str(),
			 -1);
    }

    gtk_tree_view_set_model(GTK_TREE_VIEW(ibx.list), GTK_TREE_MODEL(mod));
  }
  
  Inbox::Inbox(Ctx &ctx):
    View(ctx, "Inbox"),
    list(gtk_tree_view_new()) {
    init_list(*this);
    gtk_box_pack_start(GTK_BOX(panel), list, true, true, 0);
    
    close = gtk_button_new_with_mnemonic("_Close");
    gtk_widget_set_halign(close, GTK_ALIGN_END);
    g_signal_connect(close, "clicked", G_CALLBACK(on_close), this);
    gtk_box_pack_start(GTK_BOX(panel), close, false, false, 5);
  }

  void Inbox::focus() {
    gtk_widget_grab_focus(list);
  }
}}
