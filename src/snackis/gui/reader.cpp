#include "snackis/ctx.hpp"
#include "snackis/invite.hpp"
#include "snackis/core/fmt.hpp"
#include "snackis/gui/decrypt.hpp"
#include "snackis/gui/encrypt.hpp"
#include "snackis/gui/feed_search.hpp"
#include "snackis/gui/feed_view.hpp"
#include "snackis/gui/gui.hpp"
#include "snackis/gui/inbox.hpp"
#include "snackis/gui/post_search.hpp"
#include "snackis/gui/post_view.hpp"
#include "snackis/gui/project_view.hpp"
#include "snackis/gui/reader.hpp"
#include "snackis/gui/task_view.hpp"
#include "snackis/gui/widget.hpp"

namespace snackis {
namespace gui {
  static void init_cmds(Reader &rdr) {
    Ctx &ctx(rdr.ctx);

    rdr.cmds.emplace("clear", [&ctx](auto args) {
	if (!args.empty()) {
	  log(ctx, "Invalid number of arguments, syntax: clear");
	  return false;
	}
	
	clear(*console);
	return true;
      });

    rdr.cmds.emplace("decrypt", [&ctx](auto args) {
	if (!args.empty()) {
	  log(ctx, "Invalid number of arguments, syntax: decrypt");
	  return false;
	}
	
	Decrypt *v = new Decrypt(ctx);
	push_view(*v);
	return true;
      });

    rdr.cmds.emplace("encrypt", [&ctx](auto args) {
	if (!args.empty()) {
	  log(ctx, "Invalid number of arguments, syntax: encrypt");
	  return false;
	}
	
	Encrypt *v = new Encrypt(ctx);
	push_view(*v);
	return true;
      });

    rdr.cmds.emplace("feed", [&ctx](auto args) {
	if (!args.empty()) {
	  log(ctx, "Invalid number of arguments, syntax: feed");
	  return false;
	}
	
	FeedView *v = new FeedView(Feed(ctx));
	push_view(*v);
	return true;
      });

    rdr.cmds.emplace("feed-search", [&ctx](auto args) {
	if (!args.empty()) {
	  log(ctx, "Invalid number of arguments, syntax: feed-search");
	  return false;
	}
	
	FeedSearch *v = new FeedSearch(ctx);
	push_view(*v);
	return true;
      });

    rdr.cmds.emplace("fetch", [&ctx](auto args) {
	if (!args.empty()) {
	  log(ctx, "Invalid number of arguments, syntax: fetch");
	  return false;
	}

	ctx.fetch_cond.notify_one();
	return true;
      });

    rdr.cmds.emplace("inbox", [&ctx](auto args) {
	if (!args.empty()) {
	  log(ctx, "Invalid number of arguments, syntax: inbox");
	  return false;
	}

	if (ctx.db.inbox.recs.empty()) {
	  log(ctx, "Inbox is empty");
	  return true;
	}
	
	Inbox *v = new Inbox(ctx);
	push_view(*v);
	return true;
      });

    rdr.cmds.emplace("invite", [&ctx](auto args) {
	if (args.size() != 1) {
	  log(ctx, "Invalid number of arguments, syntax: invite foo@bar.com");
	  return false;
	}
	
	db::Trans trans(ctx);
	Invite inv(ctx, args[0]);
	load(ctx.db.invites, inv);
	send(inv);	
	log(ctx, fmt("Saved new invite: %0", inv.to));
	db::commit(trans);
	return true;
      });

    rdr.cmds.emplace("lock", [&ctx](auto args) {
	if (!args.empty()) {
	  log(ctx, "Invalid number of arguments, syntax: lock");
	  return false;
	}
	gtk_widget_hide(left_panel);
	Login *v = new Login(ctx);
	push_view(*v);
	return true;
      });
    
    rdr.cmds.emplace("post", [&ctx](auto args) {
	if (!args.empty()) {
	  log(ctx, "Invalid number of arguments, syntax: post");
	  return false;
	}
	
	PostView *v = new PostView(Post(ctx));
	push_view(*v);
	return true;
      });

    rdr.cmds.emplace("post-search", [&ctx](auto args) {
	if (!args.empty()) {
	  log(ctx, "Invalid number of arguments, syntax: post-search");
	  return false;
	}
	
	PostSearch *v = new PostSearch(ctx);
	push_view(*v);
	return true;
      });

    rdr.cmds.emplace("project", [&ctx](auto args) {
	if (!args.empty()) {
	  log(ctx, "Invalid number of arguments, syntax: project");
	  return false;
	}
	
	ProjectView *v = new ProjectView(Project(ctx));
	push_view(*v);
	return true;
      });
    
    rdr.cmds.emplace("send", [&ctx](auto args) {
	if (!args.empty()) {
	  log(ctx, "Invalid number of arguments, syntax: send");
	  return false;
	}

	if (ctx.db.outbox.recs.empty()) {
	  log(ctx, "Nothing to send");
	  return false;
	}
	
	ctx.send_cond.notify_one();
	return true;
      });

    rdr.cmds.emplace("setup", [&ctx](auto args) {
	if (!args.empty()) {
	  log(ctx, "Invalid number of arguments, syntax: setup");
	  return false;
	}
	
	Setup *v = new Setup(ctx);
	push_view(*v);
	return true;
      });

    rdr.cmds.emplace("todo", [&ctx](auto args) {
	if (args.size() > 1) {
	  log(ctx, "Invalid number of arguments, syntax: todo");
	  return false;
	}

	if (args.empty()) {  
	} else {
	  Task task(ctx);
	  task.name = args[0];
	  auto v(new TaskView(task));
	  v->queue.emplace(todo_queue(ctx));
	  push_view(*v);
	}
	
	return true;
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
      gtk_list_store_set(mod, &iter, 0, cmd.first.c_str(), -1);
    }

    gtk_entry_completion_set_model(comp, GTK_TREE_MODEL(mod));
    
  }
  
  static std::pair<str, std::vector<str>> parse_cmd(Reader &rdr, const str &in) {
    InStream in_words(in);
    str id;
    in_words >> id;
    std::vector<str> args;
    str arg;
    while (in_words >> arg) { args.push_back(arg); }
    return std::make_pair(id, args);
  }
  
  static opt<Reader::Cmd> find_cmd(Reader &rdr, const str &in) {
    str _in(in);
    
    if (_in == "") {
      if (!rdr.last_cmd) { return nullopt; }
      _in = *rdr.last_cmd;
    }

    auto parsed(parse_cmd(rdr, _in));

    auto found(rdr.cmds.find(parsed.first));
    if (found == rdr.cmds.end()) { return nullopt; }
    return found->second; 
  }

  static bool exec_cmd(Reader &rdr, const str &in) {
    Ctx &ctx(rdr.ctx);

    try {
      auto parsed(parse_cmd(rdr, in));
      auto cmd(find_cmd(rdr, parsed.first));
      
      if (!cmd){
	log(ctx, fmt("Unknown command: '%0'", in));
	return false;
      }
      
      if (!(*cmd)(parsed.second)) { return false; }
      rdr.last_cmd = in;
    } catch (const std::exception &e) {
      log(ctx, fmt("Error while executing command '%0':\n%1", in, e.what()));
      return false;
    }
    
    return true;
  }

  static void on_activate(GtkWidget *_, Reader *rdr) {
    const str in(gtk_entry_get_text(GTK_ENTRY(rdr->entry)));
    auto cmd(find_cmd(*rdr, in));
    if (cmd) { gtk_entry_set_text(GTK_ENTRY(rdr->entry), ""); }
    if (!exec_cmd(*rdr, in)) { gtk_widget_grab_focus(rdr->entry); }
  }

  Reader::Reader(Ctx &ctx): ctx(ctx), entry(gtk_entry_new()) {
    init_cmds(*this);
    init_completion(*this);
    add_style(entry, "reader");
    gtk_widget_set_margin_start(entry, 5);
    gtk_widget_set_margin_end(entry, 5);
    g_signal_connect(entry, "activate", G_CALLBACK(on_activate), this);
  }
  
  GtkWidget *Reader::ptr() {
    return entry;
  }
}}
