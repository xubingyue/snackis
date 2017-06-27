#include "snackis/ctx.hpp"
#include "snackis/gui/feed_view.hpp"
#include "snackis/gui/gui.hpp"
#include "snackis/gui/inbox.hpp"
#include "snackis/gui/reader.hpp"
#include "snackis/gui/peer_view.hpp"

namespace snackis {
namespace gui {
  enum Cols { COL_PTR=0, COL_FROM, COL_INFO };
  
  static void on_close(gpointer *_, Inbox *v) {
    pop_view(*v);
  }

  static void sel_changed(Inbox &v) {
    bool sel(get_sel_rec<Msg>(GTK_TREE_VIEW(v.list)));
    gtk_widget_set_sensitive(v.accept, sel);
    gtk_widget_set_sensitive(v.reject, sel);
  }
  
  static void on_accept(gpointer *_, Inbox *v) {
    TRACE("Inbox accept");
    Ctx &ctx(v->ctx);
    db::Trans trans(ctx);
    auto iter(*get_sel_iter(GTK_TREE_VIEW(v->list)));
    auto &msg_rec(*get_sel_rec<Msg>(GTK_TREE_VIEW(v->list), iter));
    Msg msg(ctx, msg_rec);

    if (msg.type == Msg::INVITE) {
      Peer peer(accept_invite(msg));
      log(ctx, fmt("Invite accepted: %0", msg.from));
      auto pv(new PeerView(peer));
      push_view(*pv);
    } else if (msg.type == Msg::ACCEPT) {
      Peer peer(invite_accepted(msg));
      auto pv(new PeerView(peer));
      push_view(*pv);
    } else if (msg.type == Msg::POST) {
      Feed feed(msg);
      db::insert(ctx.db.feeds, feed);
      Post post(msg);
      db::insert(ctx.db.posts, post);
      Peer peer(get_peer_id(ctx, post.by_id));
      auto fv(new FeedView(feed));
      push_view(*fv);
    } else {
	log(ctx, fmt("Invalid message type: %0", msg.type));
    }

    db::erase(ctx.db.inbox, msg_rec);
    db::commit(trans);
    gtk_list_store_remove(v->msgs, &iter);    
    sel_changed(*v);
  }

  static void on_reject(gpointer *_, Inbox *v) {
    TRACE("Inbox reject");
    Ctx &ctx(v->ctx);
    db::Trans trans(ctx);
    auto iter(*get_sel_iter(GTK_TREE_VIEW(v->list)));
    auto &msg_rec(*get_sel_rec<Msg>(GTK_TREE_VIEW(v->list), iter));
    Msg msg(ctx, msg_rec);

    if (msg.type == Msg::INVITE) {
	reject_invite(msg);
	log(ctx, fmt("Invite rejected: %0", msg.from));
    } else if (msg.type == Msg::ACCEPT || msg.type == Msg::POST) {
      // Nothing to do here
    } else {
	log(ctx, fmt("Invalid message type: %0", msg.type));
    }

    db::erase(ctx.db.inbox, msg_rec);
    db::commit(trans);
    gtk_list_store_remove(v->msgs, &iter);
    sel_changed(*v);
  }
  
  static void on_select(gpointer *_, Inbox *v) {
    sel_changed(*v);
  }
  
  static void init_list(Inbox &v) {
    add_col(GTK_TREE_VIEW(v.list), "From", COL_FROM);
    add_col(GTK_TREE_VIEW(v.list), "Message", COL_INFO, true);
    Ctx &ctx(v.ctx);
    std::vector<UId> rem;
    
    for(auto key = ctx.db.inbox_sort.recs.begin();
	key != ctx.db.inbox_sort.recs.end();
	key++) {
      auto &rec(db::get(ctx.db.inbox, *key));
      Msg msg(ctx, rec);

      GtkTreeIter iter;
      gtk_list_store_append(v.msgs, &iter);

      opt<Peer> peer(find_peer_id(ctx, msg.from_id));
      const str from(fmt("%0\n%1",
			 peer ? peer->name.c_str() : msg.peer_name.c_str(),
			 msg.from.c_str()));
		    
      gtk_list_store_set(v.msgs, &iter,
			 COL_PTR, &rec,
			 COL_FROM, from.c_str(),
			 -1);
      
      if (msg.type == Msg::INVITE) {
	gtk_list_store_set(v.msgs, &iter, COL_INFO, "Invite", -1);
      } else if (msg.type == Msg::ACCEPT) {
	gtk_list_store_set(v.msgs, &iter, COL_INFO, "Invite accepted", -1);
      } else if (msg.type == Msg::REJECT) {
	gtk_list_store_set(v.msgs, &iter, COL_INFO, "Invite rejected", -1);
      } else if (msg.type == Msg::POST) {
	gtk_list_store_set(v.msgs, &iter, COL_INFO,
			   fmt("New feed: %0", msg.feed_name).c_str(),
			   -1);
      } else {
	gtk_list_store_set(v.msgs, &iter, COL_INFO,
			   fmt("Invalid message type: %0", msg.type).c_str(),
			   -1);
      }
    }

    g_signal_connect(gtk_tree_view_get_selection(GTK_TREE_VIEW(v.list)),
		     "changed",
		     G_CALLBACK(on_select),
		     &v);
  }
  
  Inbox::Inbox(Ctx &ctx):
    View(ctx, "Inbox"),
    msgs(gtk_list_store_new(3,
			    G_TYPE_POINTER,
			    G_TYPE_STRING, G_TYPE_STRING)),
    list(gtk_tree_view_new_with_model(GTK_TREE_MODEL(msgs)))
  { }

  void Inbox::init() {
    init_list(*this);
    gtk_box_pack_start(GTK_BOX(panel), list, true, true, 0);

    GtkWidget *list_btns = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 5);
    gtk_container_add(GTK_CONTAINER(panel), list_btns);
        
    accept = gtk_button_new_with_mnemonic("_Accept Message");
    g_signal_connect(accept, "clicked", G_CALLBACK(on_accept), this);
    gtk_container_add(GTK_CONTAINER(list_btns), accept);
    
    reject = gtk_button_new_with_mnemonic("_Reject Message");
    g_signal_connect(reject, "clicked", G_CALLBACK(on_reject), this);
    gtk_container_add(GTK_CONTAINER(list_btns), reject);
    
    close = gtk_button_new_with_mnemonic("_Close Inbox");
    gtk_widget_set_halign(close, GTK_ALIGN_END);
    g_signal_connect(close, "clicked", G_CALLBACK(on_close), this);
    gtk_box_pack_start(GTK_BOX(panel), close, false, false, 5);
    sel_changed(*this);
    focused = list;
  }
}}
