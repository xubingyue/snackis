#include "snackis/ctx.hpp"
#include "snackis/gui/gui.hpp"
#include "snackis/gui/inbox.hpp"
#include "snackis/gui/reader.hpp"
#include "snackis/gui/peer_view.hpp"

namespace snackis {
namespace gui {
  enum Cols { COL_MSG=0, COL_NAME, COL_EMAIL, COL_INFO };
  
  static void on_close(gpointer *_, Inbox *v) {
    pop_view(*v);
  }

  static bool get_sel(Inbox *v, GtkTreeModel **mod, GtkTreeIter *iter) {
    GtkTreeSelection *tree_sel =
      gtk_tree_view_get_selection(GTK_TREE_VIEW(v->list));
    if (!gtk_tree_selection_get_selected(tree_sel, mod, iter)) { return false; }
    return true;
  }

  static void sel_changed(Inbox &v) {
    GtkTreeSelection *tree_sel =
      gtk_tree_view_get_selection(GTK_TREE_VIEW(v.list));
    GtkTreeModel *mod;
    GtkTreeIter iter;

    bool sel = gtk_tree_selection_get_selected(tree_sel, &mod, &iter);
    gtk_widget_set_sensitive(v.accept, sel);
    gtk_widget_set_sensitive(v.reject, sel);
  }
  
  static void on_accept(gpointer *_, Inbox *v) {
    TRACE("Inbox accept");
    Ctx &ctx(v->ctx);
    db::Trans trans(ctx);
    GtkTreeModel *mod;
    GtkTreeIter iter;
    get_sel(v, &mod, &iter);

    db::Rec<Msg> *msg_rec;
    gtk_tree_model_get(mod, &iter, COL_MSG, &msg_rec, -1);
    Msg msg(ctx, *msg_rec);

    if (msg.type == Msg::INVITE) {
      Peer peer(accept_invite(msg));
      log(ctx, fmt("Accept of %0 saved to outbox", msg.from));
      auto pv(new PeerView(peer));
      push_view(*pv);
    } else if (msg.type == Msg::ACCEPT) {
      Peer peer(invite_accepted(msg));
      auto pv(new PeerView(peer));
      push_view(*pv);
    } else {
	log(ctx, fmt("Invalid message type: %0", msg.type));
    }

    db::erase(ctx.db.inbox, *msg_rec);
    db::commit(trans);
    gtk_list_store_remove(GTK_LIST_STORE(mod), &iter);    
    sel_changed(*v);
  }

  static void on_reject(gpointer *_, Inbox *v) {
    TRACE("Inbox reject");
    Ctx &ctx(v->ctx);
    db::Trans trans(ctx);
    GtkTreeModel *mod;
    GtkTreeIter iter;
    get_sel(v, &mod, &iter);

    db::Rec<Msg> *msg_rec;
    gtk_tree_model_get(mod, &iter, COL_MSG, &msg_rec, -1);
    Msg msg(ctx, *msg_rec);

    if (msg.type == Msg::INVITE) {
	reject_invite(msg);
	log(ctx, fmt("Reject of %0 saved to outbox", msg.from));
    } else if (msg.type == Msg::ACCEPT) {
      // Nothing to do here
    } else {
	log(ctx, fmt("Invalid message type: %0", msg.type));
    }

    db::erase(ctx.db.inbox, *msg_rec);
    db::commit(trans);
    gtk_list_store_remove(GTK_LIST_STORE(mod), &iter);
    sel_changed(*v);
  }
  
  static void on_select(gpointer *_, Inbox *v) {
    sel_changed(*v);
  }
  
  static void init_list(Inbox &v) {
    GtkCellRenderer *rend;

    rend = gtk_cell_renderer_text_new();
    auto name_col(gtk_tree_view_column_new_with_attributes("From",
							   rend,
							   "text", COL_NAME,
							   nullptr));
    gtk_tree_view_append_column(GTK_TREE_VIEW(v.list), name_col);    
  
    rend = gtk_cell_renderer_text_new();
    auto email_col(gtk_tree_view_column_new_with_attributes("",
							    rend,
							    "text", COL_EMAIL,
							    nullptr));
    gtk_tree_view_append_column(GTK_TREE_VIEW(v.list), email_col);

    rend = gtk_cell_renderer_text_new();
    auto info_col(gtk_tree_view_column_new_with_attributes("Message",
							   rend,
							   "text", COL_INFO,
							   nullptr));
    gtk_tree_view_column_set_expand(GTK_TREE_VIEW_COLUMN(info_col), true);
    gtk_tree_view_append_column(GTK_TREE_VIEW(v.list), info_col);
    auto mod(gtk_list_store_new(4,
				G_TYPE_POINTER,
				G_TYPE_STRING, G_TYPE_STRING, G_TYPE_STRING));
    Ctx &ctx(v.ctx);
    std::vector<UId> rem;
    
    for(const auto &msg_rec: ctx.db.inbox.recs) {
      Msg msg(ctx, msg_rec);

      GtkTreeIter iter;
      gtk_list_store_append(mod, &iter);
      
      opt<Peer> peer(find_peer_email(ctx, msg.from));
      gtk_list_store_set(mod, &iter,
			 COL_MSG, &msg_rec,
			 COL_NAME, peer ? peer->name.c_str() : msg.peer_name.c_str(),
			 COL_EMAIL, msg.from.c_str(),
			 -1);
      
      if (msg.type == Msg::INVITE) {
	gtk_list_store_set(mod, &iter, COL_INFO, "Invite", -1);
      } else if (msg.type == Msg::ACCEPT) {
	gtk_list_store_set(mod, &iter, COL_INFO, "Invite accepted", -1);
      } else if (msg.type == Msg::REJECT) {
	gtk_list_store_set(mod, &iter, COL_INFO, "Invite rejected", -1);
      } else {
	gtk_list_store_set(mod, &iter, COL_INFO,
			   fmt("Invalid message type: %0", msg.type).c_str(),
			   -1);
      }
    }

    gtk_tree_view_set_model(GTK_TREE_VIEW(v.list), GTK_TREE_MODEL(mod));
    g_signal_connect(gtk_tree_view_get_selection(GTK_TREE_VIEW(v.list)),
		     "changed",
		     G_CALLBACK(on_select),
		     &v);
  }
  
  Inbox::Inbox(Ctx &ctx):
    View(ctx, "Inbox"),
    list(gtk_tree_view_new()) {
    init_list(*this);
    gtk_box_pack_start(GTK_BOX(panel), list, true, true, 0);

    GtkWidget *list_btns = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 5);
    gtk_container_add(GTK_CONTAINER(panel), list_btns);
        
    accept = gtk_button_new_with_mnemonic("_Accept");
    g_signal_connect(accept, "clicked", G_CALLBACK(on_accept), this);
    gtk_container_add(GTK_CONTAINER(list_btns), accept);
    
    reject = gtk_button_new_with_mnemonic("_Reject");
    g_signal_connect(reject, "clicked", G_CALLBACK(on_reject), this);
    gtk_container_add(GTK_CONTAINER(list_btns), reject);
    
    close = gtk_button_new_with_mnemonic("_Close Inbox");
    gtk_widget_set_halign(close, GTK_ALIGN_END);
    g_signal_connect(close, "clicked", G_CALLBACK(on_close), this);
    gtk_box_pack_start(GTK_BOX(panel), close, false, false, 5);
    sel_changed(*this);
  }

  void Inbox::focus() {
    gtk_widget_grab_focus(list);
  }
}}
