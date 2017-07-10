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
  
  static void on_cancel(gpointer *_, Inbox *v) {
    pop_view(*v);
  }

  static void on_activate(GtkTreeView *treeview,
			  GtkTreePath *path,
			  GtkTreeViewColumn *col,
			  Inbox *v) {
    TRY(try_activate);
    Ctx &ctx(v->ctx);
    db::Trans trans(ctx);
    auto it(*get_sel_iter(GTK_TREE_VIEW(v->lst)));
    auto rec(get_rec<Msg>(GTK_TREE_VIEW(v->lst), it));
    CHECK(rec != nullptr, _);
    Msg msg(ctx, *rec);

    if (msg.type == Msg::INVITE) {
      auto pv(new PeerView(Peer(msg)));

      pv->on_save.push_back([&ctx, msg, rec]() {
	  send_accept(msg);
	  log(ctx, fmt("Invite accepted: %0", msg.from));
	});

      pv->on_cancel.push_back([&ctx, msg, rec]() {
	  reject_invite(msg);
	  log(ctx, fmt("Invite rejected: %0", msg.from));
	});

      push_view(*pv);
    } else if (msg.type == Msg::ACCEPT) {
      auto pv(new PeerView(Peer(msg)));

      pv->on_save.push_back([&ctx, msg, rec]() {
	  invite_accepted(msg);
	});

      push_view(*pv);
    } else if (msg.type == Msg::REJECT) {
      invite_rejected(msg);
    } else if (msg.type == Msg::POST) {
      Feed fd(ctx, msg.feed);
      auto fd_fnd(find_feed_id(ctx, fd.id));
      
      if (fd_fnd) {
	copy(*fd_fnd, msg);
	auto fv(new FeedView(*fd_fnd));
	
	Post ps(ctx, msg.post);
	auto ps_fnd(find_post_id(ctx, ps.id));

	if (ps_fnd) {
	  copy(*ps_fnd, msg);
	  auto pv(new PostView(*ps_fnd));
	  push_view(*pv);
	} else {
	  auto pv(new PostView(Post(msg)));
	  push_view(*pv);
	}

	push_view(*fv);
      } else {
	auto fv(new FeedView(Feed(msg)));

	fv->on_save.push_back([msg]() {
	    auto pv(new PostView(Post(msg)));
	    push_view(*pv);
	  });

	push_view(*fv);	
      }
    } else if (msg.type == Msg::TASK) {
      Project prj(ctx, msg.project);
      auto prj_fnd(find_project_id(ctx, prj.id));
      
      if (prj_fnd) {
	copy(*prj_fnd, msg);
	auto pv(new ProjectView(*prj_fnd));

	Task tsk(ctx, msg.task);
	auto tsk_fnd(find_task_id(ctx, *db::get(msg.task, ctx.db.task_id)));

	if (tsk_fnd) {
	  copy(*tsk_fnd, msg);
	  auto tv(new TaskView(*tsk_fnd));
	  push_view(*tv);
	} else {
	  auto tv(new TaskView(Task(msg)));
	  push_view(*tv);
	}

	push_view(*pv);
      } else {
	auto pv(new ProjectView(Project(msg)));
	
	pv->on_save.push_back([msg]() {
	    View::stack.push_back(new TaskView(Task(msg)));
	  });

	push_view(*pv);
      }      
    } else {
	log(ctx, fmt("Invalid message type: %0", msg.type));
    }
    
    gtk_list_store_remove(v->msgs, &it);
    db::erase(ctx.db.inbox, *rec);
    db::commit(trans, fmt("Handled Message: %0", id_str(msg)));
  }
  
  static void init_lst(Inbox &v) {
    add_col(GTK_TREE_VIEW(v.lst), "From", COL_FROM);
    add_col(GTK_TREE_VIEW(v.lst), "Message", COL_INFO, true);
    Ctx &ctx(v.ctx);
    
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
	Feed fd(ctx, msg.feed);
	Post ps(ctx, msg.post);
	auto fd_fnd(find_feed_id(ctx, fd.id));

	if (fd_fnd) {  
	  auto ps_fnd(find_post_id(ctx, ps.id));
	  
	  if (ps_fnd) {
	    gtk_list_store_set(v.msgs, &iter,
			       COL_INFO,
			       fmt("Post %0 updated", id_str(ps)).c_str(),
			       -1);
	  } else {
	    gtk_list_store_set(v.msgs, &iter,
			       COL_INFO,
			       fmt("New post in feed %0:\n%1",
				   id_str(fd), ps.body).c_str(),
			       -1);
	  }
	} else {
	  gtk_list_store_set(v.msgs, &iter,
			     COL_INFO,
			     fmt("New feed:\n%0", fd.name).c_str(),
			     -1);
	}
      } else if (msg.type == Msg::TASK) {
	Project prj(ctx, msg.project);
	Task tsk(ctx, msg.task);
	auto prj_fnd(find_project_id(ctx, prj.id));

	if (prj_fnd) {  
	  auto tsk_fnd(find_task_id(ctx, tsk.id));
	  
	  if (tsk_fnd) {
	    gtk_list_store_set(v.msgs, &iter,
			       COL_INFO,
			       fmt("Task %0 updated", id_str(tsk)).c_str(),
			       -1);
	  } else {
	    gtk_list_store_set(v.msgs, &iter,
			       COL_INFO,
			       fmt("New task in project %0:\n%1",
				   id_str(prj), tsk.name).c_str(),
			       -1);
	  }
	} else {
	  gtk_list_store_set(v.msgs, &iter,
			     COL_INFO,
			     fmt("New project:\n%0", prj.name).c_str(),
			     -1);
	}	  
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
    cancel_btn(gtk_button_new_with_mnemonic("_Cancel"))
  {
    GtkWidget *lbl;

    init_lst(*this);
    g_signal_connect(lst, "row-activated", G_CALLBACK(on_activate), this);
    gtk_container_add(GTK_CONTAINER(panel), gtk_widget_get_parent(lst));

    lbl = gtk_label_new(fmt("Press Return or double-click "
			    "to handle Message").c_str());
    gtk_container_add(GTK_CONTAINER(panel), lbl);
     
    gtk_widget_set_margin_top(cancel_btn, 10);
    gtk_widget_set_halign(cancel_btn, GTK_ALIGN_END);
    g_signal_connect(cancel_btn, "clicked", G_CALLBACK(on_cancel), this);
    gtk_box_pack_start(GTK_BOX(panel), cancel_btn, false, false, 0);
    focused = lst;
  }
}}
