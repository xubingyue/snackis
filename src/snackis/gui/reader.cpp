#include <cctype>
#include "snackis/ctx.hpp"
#include "snackis/invite.hpp"
#include "snackis/snackis.hpp"
#include "snackis/core/fmt.hpp"
#include "snackis/gui/decrypt.hpp"
#include "snackis/gui/encrypt.hpp"
#include "snackis/gui/feed_search.hpp"
#include "snackis/gui/feed_view.hpp"
#include "snackis/gui/gui.hpp"
#include "snackis/gui/inbox.hpp"
#include "snackis/gui/peer_search.hpp"
#include "snackis/gui/peer_view.hpp"
#include "snackis/gui/post_search.hpp"
#include "snackis/gui/post_view.hpp"
#include "snackis/gui/project_search.hpp"
#include "snackis/gui/project_view.hpp"
#include "snackis/gui/reader.hpp"
#include "snackis/gui/script_search.hpp"
#include "snackis/gui/script_view.hpp"
#include "snackis/gui/task_search.hpp"
#include "snackis/gui/task_view.hpp"
#include "snackis/gui/todo.hpp"
#include "snackis/gui/undo.hpp"
#include "snackis/gui/widget.hpp"

namespace snackis {
namespace gui {
  static gboolean on_key(gpointer _, GdkEventKey *ev, Reader *rdr) {
    if (ev->keyval != GDK_KEY_Tab) { return false; }

    const str in(gtk_entry_get_text(GTK_ENTRY(rdr->entry)));
    if (in.empty()) { return true; }
    auto fnd(rdr->cmds.lower_bound(in));
    if (fnd == rdr->cmds.end()) { return true; }
    const str fnd_str(*fnd);
    if (fnd_str.find(in) != 0) { return true; }
    size_t i(fnd_str.size());
      
    while (fnd != rdr->cmds.end() && fnd->find(in) == 0) {
      const str nxt_str(*fnd);
      size_t j(0);
      
      while (j < fnd_str.size() && j < nxt_str.size()) {
	if (fnd_str[j] != nxt_str[j]) { break; }
	j++;
      }
      
      i = std::min(i, j);
      fnd++;
    }
      
    gtk_entry_set_text(GTK_ENTRY(rdr->entry), fnd_str.substr(0, i).c_str());
    gtk_editable_set_position(GTK_EDITABLE(rdr->entry), i);
    return true;
  }

  static void add_cmd(Reader &rdr,
		      const str &id,
		      const snabel::ArgTypes &args,
		      Reader::Cmd cmd) {
    CHECK(rdr.cmds.insert(id).second, _);
    snabel::Func &f(add_func(rdr.exec.main, id));
    add_imp(f, args, rdr.exec.void_type,
	    [cmd](snabel::Scope &scp, snabel::FuncImp &fn, const snabel::Args &args) {
	      cmd(args);
	    });
  };
  
  template <typename SearchT, typename ViewT, typename RecT>
  static void init_id_search(Reader &rdr, const str &id) {
    Ctx &ctx(rdr.ctx);

    add_cmd(rdr, id, {&rdr.exec.str_type}, [&ctx, id](auto args) {
	auto *v(new SearchT(ctx));
	auto id(snabel::get<str>(args[0]));
	gui::set_str(GTK_ENTRY(v->id_fld), id);
	
	if (find(*v) == 1) {
	  auto rec(first_rec(*v));
	  CHECK(rec != nullptr, _);
	  push_view(new ViewT(RecT(ctx, *rec)));
	  delete v;
	} else {
	  push_view(v);
	}
      });
  }

  template <typename ViewT, typename RecT>
  static void init_new(Reader &rdr, const str &id) {
    Ctx &ctx(rdr.ctx);

    add_cmd(rdr, fmt("%0-new", id), {}, [&ctx, id](auto args) {
	push_view(new ViewT(RecT(ctx)));
      });
  }

  template <typename ViewT>
  static void init_search(Reader &rdr, const str &id) {
    Ctx &ctx(rdr.ctx);

    add_cmd(rdr, fmt("%0-search", id), {}, [&ctx, id](auto args) {
	push_view(new ViewT(ctx));
      });
  }
  
  static void init_cmds(Reader &rdr) {
    Ctx &ctx(rdr.ctx);

    add_cmd(rdr, "clear", {}, [&ctx](auto args) {
	clear(*console);
      });

    add_cmd(rdr, "decrypt", {}, [&ctx](auto args) {
	push_view(new Decrypt(ctx));
      });

    add_cmd(rdr, "rewrite!", {}, [&ctx](auto args) {
	log(ctx, "Rewriting database...");
	log(ctx, fmt("Finished rewriting, %0k reclaimed",
		     rewrite_db(ctx) / 1000));
      });

    add_cmd(rdr, "encrypt", {}, [&ctx](auto args) {
	push_view(new Encrypt(ctx));
      });

    init_id_search<ScriptSearch, ScriptView, Script>(rdr, "script");
    init_new<ScriptView, Script>(rdr, "script");
    init_search<ScriptSearch>(rdr, "script");

    init_id_search<FeedSearch, FeedView, Feed>(rdr, "feed");
    init_new<FeedView, Feed>(rdr, "feed");
    init_search<FeedSearch>(rdr, "feed");

    add_cmd(rdr, "fetch", {}, [&ctx](auto args) {
	imap_worker->go.notify_one();
      });

    add_cmd(rdr, "inbox", {}, [&ctx](auto args) {
	refresh(ctx);

	if (ctx.db.inbox.recs.empty()) {
	  log(ctx, "Inbox is empty");
	} else {
	  if (!inbox) { inbox.reset(new Inbox(ctx)); }
	  push_view(inbox.get());
	}
      });

    add_cmd(rdr, "invite", {&rdr.exec.str_type}, [&ctx](auto args) {
	db::Trans trans(ctx);
	TRY(try_invite);
	Invite inv(ctx, snabel::get<str>(args[0]));
	load(ctx.db.invites, inv);
	send(inv);
	
	if (try_invite.errors.empty()) {
	  log(ctx, fmt("Saved new invite: %0", inv.to));
	  db::commit(trans, fmt("Created invite: ", inv.to));
	}
      });

    add_cmd(rdr, "lock", {}, [&ctx](auto args) {
	gtk_widget_hide(left_panel);
	push_view(new Login(ctx));
      });

    init_id_search<PeerSearch, PeerView, Peer>(rdr, "peer");
    init_search<PeerSearch>(rdr, "peer");
    
    init_id_search<PostSearch, PostView, Post>(rdr, "post");
    init_new<PostView, Post>(rdr, "post");
    init_search<PostSearch>(rdr, "post");

    init_id_search<ProjectSearch, ProjectView, Project>(rdr, "project");
    init_new<ProjectView, Project>(rdr, "project");
    init_search<ProjectSearch>(rdr, "project");

    add_cmd(rdr, "send", {}, [&ctx](auto args) {
	if (ctx.db.outbox.recs.empty()) {
	  log(ctx, "Nothing to send");
	} else {
	  smtp_worker->go.notify_one();
	}
      });

    add_cmd(rdr, "setup", {}, [&ctx](auto args) {
	if (!setup) { setup.reset(new Setup(ctx)); }
	push_view(setup.get());
      });

    init_id_search<TaskSearch, TaskView, Task>(rdr, "task");
    init_new<TaskView, Task>(rdr, "task");
    init_search<TaskSearch>(rdr, "task");

    add_cmd(rdr, "todo", {}, [&ctx](auto args) {
	if (!todo) { todo.reset(new Todo(ctx)); }
	push_view(todo.get());
      });
    
    add_cmd(rdr, "undo", {}, [&ctx](auto args) {
	if (ctx.undo_stack.empty()) {
	  log(ctx, "Nothing to undo");
	} else {
	  if (!undo) { undo.reset(new Undo(ctx)); }
	  push_view(undo.get());
	}
      });
  }
  
  static void init_completion(Reader &rdr) {
    auto comp(gtk_entry_completion_new());
    gtk_entry_completion_set_text_column(comp, 0);
    gtk_entry_set_completion(GTK_ENTRY(rdr.entry), comp);
    auto mod(gtk_list_store_new(1, G_TYPE_STRING));
    
    for(auto &cmd: rdr.cmds) {
      GtkTreeIter iter;
      gtk_list_store_append(mod, &iter);
      gtk_list_store_set(mod, &iter, 0, cmd.c_str(), -1);
    }

    gtk_entry_completion_set_model(comp, GTK_TREE_MODEL(mod));
    
  }
  
  static bool exec_cmd(Reader &rdr, const str &in) {
    TRY(try_exec);

    auto &cor(rdr.exec.main);
    snabel::compile(cor, in, false);    
    begin_scope(cor, false);
    snabel::run(cor);
    end_scope(cor);
     
    if (try_exec.errors.empty()) {
      rdr.last_cmd = in;
      gtk_entry_set_placeholder_text(GTK_ENTRY(rdr.entry), in.c_str());
    }
    
    return try_exec.errors.empty();
  }

  static void on_activate(GtkWidget *_, Reader *rdr) {
    const str in(gtk_entry_get_text(GTK_ENTRY(rdr->entry)));

    if (exec_cmd(*rdr, in)) {
      auto res(peek(rdr->exec.main));
      set_str(GTK_ENTRY(rdr->entry), res ? fmt_arg(*res) : "");
    }
    
    if (!get_str(GTK_ENTRY(rdr->entry)).empty()) {
      gtk_widget_grab_focus(rdr->entry);
    }
  }

  Reader::Reader(Ctx &ctx): ctx(ctx), entry(gtk_entry_new()) {
    init_cmds(*this);
    init_completion(*this);
    add_style(entry, "reader");
    gtk_widget_set_margin_start(entry, 5);
    gtk_widget_set_margin_end(entry, 5);
    g_signal_connect(entry, "activate", G_CALLBACK(on_activate), this);

    g_signal_connect(G_OBJECT(entry),
		     "key_press_event",
		     G_CALLBACK(on_key),
		     this);
  }
  
  GtkWidget *Reader::ptr() {
    return entry;
  }
}}
