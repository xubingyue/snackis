#include "snackis/ctx.hpp"
#include "snackis/gui/feed_view.hpp"
#include "snackis/gui/gui.hpp"
#include "snackis/gui/inbox.hpp"
#include "snackis/gui/reader.hpp"
#include "snackis/gui/feed_view.hpp"
#include "snackis/gui/peer_view.hpp"
#include "snackis/gui/post_view.hpp"
#include "snackis/gui/project_view.hpp"
#include "snackis/gui/task_view.hpp"

namespace snackis {
namespace gui {
  enum Cols { COL_PTR=0, COL_FROM, COL_INFO };
  
  static void on_sel_change(gpointer *_, Inbox *v) {
    gtk_widget_set_sensitive(v->dismiss_btn, sel_count(GTK_TREE_VIEW(v->lst)) > 0);
  }
  
  static void on_cancel(gpointer *_, Inbox *v) {
    pop_view(v);
  }

  static void on_dismiss(gpointer *_, Inbox *v) {
    each_sel(GTK_TREE_VIEW(v->lst), [v](auto &it) {
	TRY(try_dismiss);
	db::Trans trans(v->ctx);
	auto rec(get_rec<Msg>(GTK_TREE_VIEW(v->lst), it));
	db::erase(v->ctx.db.inbox, *rec);
	
	if (try_dismiss.errors.empty()) {
	  db::commit(trans, nullopt);
	  gtk_list_store_remove(v->store, &it);
	}
      });
  }

  static void on_activate(GtkTreeView *treeview,
			  GtkTreePath *path,
			  GtkTreeViewColumn *col,
			  Inbox *v) {
    each_sel(treeview, [v](auto &it) {
	Ctx &ctx(v->ctx);
	db::Trans trans(ctx);
	TRY(try_activate);
	auto rec(get_rec<Msg>(GTK_TREE_VIEW(v->lst), it));
	Msg msg(ctx, *rec);
	
	if (msg.type == Msg::INVITE) {
	  const str log_msg(fmt("Accepted invite from %0", msg.from));
	  log(ctx, log_msg);
	  send_accept(msg);
	  
	  db::Trans trans(ctx);
	  Peer pr(msg);
	  db::upsert(ctx.db.peers, pr);
	  
	  auto pv(new PeerView(pr));
	  push_view(pv);
	} else if (msg.type == Msg::ACCEPT) {
	  push_view(new PeerView(get_peer_id(ctx, msg.from_id)));
	} else if (msg.type == Msg::POST) {
	  Post ps(ctx, msg.post);
	  push_view(new PostView(get_post_id(ctx, ps.id)));
	} else if (msg.type == Msg::TASK) {
	  Task tsk(ctx, msg.task);      
	  push_view(new TaskView(get_task_id(ctx, tsk.id)));
	} else {
	  log(ctx, fmt("Invalid message type: %0", msg.type));
	}

	if (try_activate.errors.empty()) {
	  gtk_list_store_remove(v->store, &it);
	  db::erase(ctx.db.inbox, msg);
	  db::commit(trans, nullopt);
	}
      });
  }

  Inbox::Inbox(Ctx &ctx):
    View(ctx, "Inbox"),
    store(gtk_list_store_new(3,
			     G_TYPE_POINTER,
			     G_TYPE_STRING, G_TYPE_STRING)),
    lst(new_tree_view(GTK_TREE_MODEL(store))),
    dismiss_btn(gtk_button_new_with_mnemonic("_Dismiss Selected")),
    cancel_btn(gtk_button_new_with_mnemonic("_Cancel"))
  {
    GtkWidget *lbl;

    add_col(GTK_TREE_VIEW(lst), "From", COL_FROM);
    add_col(GTK_TREE_VIEW(lst), "Message", COL_INFO, true);
    g_signal_connect(lst, "row-activated", G_CALLBACK(on_activate), this);
    enable_multi_sel(GTK_TREE_VIEW(lst));
    auto sel(gtk_tree_view_get_selection(GTK_TREE_VIEW(lst)));
    g_signal_connect(sel, "changed", G_CALLBACK(on_sel_change), this);
    gtk_container_add(GTK_CONTAINER(panel), gtk_widget_get_parent(lst));

    lbl = gtk_label_new(fmt("Press Return or double-click "
			    "to handle Message").c_str());
    gtk_container_add(GTK_CONTAINER(panel), lbl);

    gtk_widget_set_halign(dismiss_btn, GTK_ALIGN_START);
    g_signal_connect(dismiss_btn, "clicked", G_CALLBACK(on_dismiss), this);
    gtk_container_add(GTK_CONTAINER(panel), dismiss_btn);

    gtk_widget_set_margin_top(cancel_btn, 10);
    gtk_widget_set_halign(cancel_btn, GTK_ALIGN_END);
    g_signal_connect(cancel_btn, "clicked", G_CALLBACK(on_cancel), this);
    gtk_box_pack_start(GTK_BOX(panel), cancel_btn, false, false, 0);
    focused = lst;
  }

  void Inbox::load() {
    View::load();
    Ctx::Lock lock(ctx.mutex);
    gtk_list_store_clear(store);
    size_t cnt(0);
    
    for(auto key = ctx.db.inbox_sort.recs.begin();
	key != ctx.db.inbox_sort.recs.end();
	key++) {
      auto &rec(db::get(ctx.db.inbox, *key));
      Msg msg(ctx, rec);

      GtkTreeIter iter;
      gtk_list_store_append(store, &iter);

      opt<Peer> peer(find_peer_id(ctx, msg.from_id));
      const str from(fmt("%0\n%1",
			 peer ? peer->name.c_str() : msg.peer_name.c_str(),
			 msg.from.c_str()));
		    
      gtk_list_store_set(store, &iter,
			 COL_PTR, &rec,
			 COL_FROM, from.c_str(),
			 -1);
      
      if (msg.type == Msg::INVITE) {
	gtk_list_store_set(store, &iter, COL_INFO, "Invite", -1);
      } else if (msg.type == Msg::ACCEPT) {
	gtk_list_store_set(store, &iter, COL_INFO, "Invite accepted", -1);
      } else if (msg.type == Msg::POST) {
	Post ps(ctx, msg.post);
	Feed fd(get_feed_id(ctx, ps.feed_id));
	
	gtk_list_store_set(store, &iter,
			   COL_INFO,
			   fmt("New post in feed '%0':\n%1",
			       fd.name, ps.body).c_str(),
			   -1);
      } else if (msg.type == Msg::TASK) {
	Task tsk(ctx, msg.task);
	Project prj(get_project_id(ctx, tsk.project_id));
	
	gtk_list_store_set(store, &iter,
			   COL_INFO,
			   fmt("New task in project '%0':\n%1",
			       prj.name, tsk.name).c_str(),
			   -1);
      } else {
	gtk_list_store_set(store, &iter, COL_INFO,
			   fmt("Invalid message type: %0", msg.type).c_str(),
			   -1);
      }

      cnt++;
    }

    if (cnt) {
      sel_first(GTK_TREE_VIEW(lst));
      gtk_widget_grab_focus(lst);
    }
  }
}}
