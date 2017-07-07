#include "snackis/ctx.hpp"
#include "snackis/gui/feed_view.hpp"
#include "snackis/gui/gui.hpp"
#include "snackis/gui/inbox.hpp"
#include "snackis/gui/reader.hpp"
#include "snackis/gui/peer_view.hpp"
#include "snackis/gui/project_view.hpp"

namespace snackis {
namespace gui {
  enum Cols { COL_PTR=0, COL_FROM, COL_INFO };
  
  static void on_close(gpointer *_, Inbox *v) {
    pop_view(*v);
  }

  static void on_activate(GtkTreeView *treeview,
			  GtkTreePath *path,
			  GtkTreeViewColumn *col,
			  Inbox *v) {
    Ctx &ctx(v->ctx);
    auto iter(*get_sel_iter(GTK_TREE_VIEW(v->lst)));
    auto rec(get_sel_rec<Msg>(GTK_TREE_VIEW(v->lst), iter));
    CHECK(rec != nullptr, _);
    Msg msg(ctx, *rec);

    if (msg.type == Msg::INVITE) {
      auto pv(new PeerView(get_peer(msg)));

      pv->on_save.push_back([&ctx, msg, rec]() {
	  send_accept(msg);
	  db::erase(ctx.db.inbox, *rec);
	  log(ctx, fmt("Invite accepted: %0", msg.from));
	});

      pv->on_cancel.push_back([&ctx, msg, rec]() {
	  reject_invite(msg);
	  db::erase(ctx.db.inbox, *rec);
	  log(ctx, fmt("Invite rejected: %0", msg.from));
	});

      push_view(*pv);
    } else if (msg.type == Msg::ACCEPT) {
      auto pv(new PeerView(get_peer(msg)));

      pv->on_save.push_back([&ctx, msg, rec]() {
	  invite_accepted(msg);
	  db::erase(ctx.db.inbox, *rec);
	});

      push_view(*pv);
    } else if (msg.type == Msg::POST) {
      auto fv(new FeedView(Feed(msg)));

      fv->on_save.push_back([&ctx, msg, rec]() {
	  Post ps(msg);
	  db::insert(ctx.db.posts, ps);
	  db::erase(ctx.db.inbox, *rec);
	  auto pr(get_peer_id(ctx, msg.from_id));
	  log(ctx, fmt("New post %0 by %1:\n%2", id_str(ps), pr.name, ps.body));
	});
      
      fv->on_cancel.push_back([&ctx, rec]() {
	  db::erase(ctx.db.inbox, *rec);
	});      

      push_view(*fv);
    } else if (msg.type == Msg::TASK) {
      auto pv(new ProjectView(Project(msg)));
      pv->on_save.push_back([&ctx, msg, rec]() {
	  Task tsk(msg);
	  db::insert(ctx.db.tasks, tsk);
	  db::erase(ctx.db.inbox, *rec);
	  log(ctx, fmt("New task %0:\n%1\n%2", id_str(tsk), tsk.name, tsk.info));
	});
      pv->on_cancel.push_back([&ctx, rec]() {
	  db::erase(ctx.db.inbox, *rec);
	});      
      push_view(*pv);
    } else {
	log(ctx, fmt("Invalid message type: %0", msg.type));
    }

    gtk_list_store_remove(v->msgs, &iter);    
  }
  
  static void init_lst(Inbox &v) {
    add_col(GTK_TREE_VIEW(v.lst), "From", COL_FROM);
    add_col(GTK_TREE_VIEW(v.lst), "Message", COL_INFO);
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
      } else if (msg.type == Msg::POST) {
	gtk_list_store_set(v.msgs, &iter,
			   COL_INFO,
			   fmt("New feed:\n%0",
			       *db::get(msg.feed, ctx.db.feed_name)).c_str(),
			   -1);
      } else if (msg.type == Msg::TASK) {
	gtk_list_store_set(v.msgs, &iter,
			   COL_INFO,
			   fmt("New project:\n%0",
			       *db::get(msg.project, ctx.db.project_name)).c_str(),
			   -1);
      } else {
	gtk_list_store_set(v.msgs, &iter, COL_INFO,
			   fmt("Invalid message type: %0", msg.type).c_str(),
			   -1);
      }
    }
  }
  
  Inbox::Inbox(Ctx &ctx):
    View(ctx, "Inbox"),
    msgs(gtk_list_store_new(3,
			    G_TYPE_POINTER,
			    G_TYPE_STRING, G_TYPE_STRING)),
    lst(new_tree_view(GTK_TREE_MODEL(msgs))),
    close_btn(gtk_button_new_with_mnemonic("_Close Inbox"))
  {
    GtkWidget *lbl;

    init_lst(*this);
    g_signal_connect(lst, "row-activated", G_CALLBACK(on_activate), this);
    gtk_container_add(GTK_CONTAINER(panel), gtk_widget_get_parent(lst));

    lbl = gtk_label_new(fmt("Press Return or double-click "
			    "to handle message").c_str());
    gtk_container_add(GTK_CONTAINER(panel), lbl);
     
    gtk_widget_set_margin_top(close_btn, 10);
    gtk_widget_set_halign(close_btn, GTK_ALIGN_END);
    g_signal_connect(close_btn, "clicked", G_CALLBACK(on_close), this);
    gtk_box_pack_start(GTK_BOX(panel), close_btn, false, false, 0);
    focused = lst;
  }
}}
