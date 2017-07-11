#include <cctype>
#include "snackis/ctx.hpp"
#include "snackis/invite.hpp"
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
#include "snackis/gui/task_search.hpp"
#include "snackis/gui/task_view.hpp"
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
    const str fnd_str(fnd->first);
    if (fnd_str.find(in) != 0) { return true; }
    size_t i(fnd_str.size());
      
    while (fnd != rdr->cmds.end() && fnd->first.find(in) == 0) {
      const str nxt_str(fnd->first);
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
  
  static void init_cmds(Reader &rdr) {
    Ctx &ctx(rdr.ctx);

    rdr.cmds.emplace("clear", [&ctx](auto args) {
	if (!args.empty()) {
	  log(ctx, "Invalid number of arguments, syntax: clear");
	  return false;
	}
	
	clear(*console);
	ctx.undo_stack.clear();
	return true;
      });

    rdr.cmds.emplace("decrypt", [&ctx](auto args) {
	if (!args.empty()) {
	  log(ctx, "Invalid number of arguments, syntax: decrypt");
	  return false;
	}
	
	push_view(new Decrypt(ctx));
	return true;
      });

    rdr.cmds.emplace("defrag", [&ctx](auto args) {
	if (!args.empty()) {
	  log(ctx, "Invalid number of arguments, syntax: defrag");
	  return false;
	}

	log(ctx, "Defragmenting database...");
	log(ctx, fmt("Finished defragmenting, %0k reclaimed",
		     db::defrag(ctx) / 1000));
	return true;
      });

    rdr.cmds.emplace("encrypt", [&ctx](auto args) {
	if (!args.empty()) {
	  log(ctx, "Invalid number of arguments, syntax: encrypt");
	  return false;
	}
	
	push_view(new Encrypt(ctx));
	return true;
      });

    rdr.cmds.emplace("feed", [&ctx](auto args) {
	if (args.size() != 1) {
	  log(ctx, "Invalid number of arguments, syntax: feed ea362b58");
	  return false;
	}
	
	FeedSearch *v = new FeedSearch(ctx);
	auto id(args.back());
	gui::set_str(GTK_ENTRY(v->id_fld), id);

	if (find(*v) == 1) {
	  auto rec(first_rec(*v));
	  CHECK(rec != nullptr, _);
	  push_view(new FeedView(Feed(ctx, *rec)));
	  delete v;
	} else {
	  push_view(v);
	}
	
	return true;
      });

    rdr.cmds.emplace("feed-new", [&ctx](auto args) {
	if (!args.empty()) {
	  log(ctx, "Invalid number of arguments, syntax: feed");
	  return false;
	}
	
	push_view(new FeedView(Feed(ctx)));
	return true;
      });

    rdr.cmds.emplace("feed-search", [&ctx](auto args) {
	if (!args.empty()) {
	  log(ctx, "Invalid number of arguments, syntax: feed-search");
	  return false;
	}
	
	push_view(new FeedSearch(ctx));
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

	if (!inbox) { inbox.reset(new Inbox(ctx)); }
	push_view(inbox.get());
	return true;
      });

    rdr.cmds.emplace("invite", [&ctx](auto args) {
	if (args.size() != 1) {
	  log(ctx, "Invalid number of arguments, syntax: invite foo@bar.com");
	  return false;
	}
	
	db::Trans trans(ctx);
	TRY(try_invite);
	Invite inv(ctx, args[0]);
	load(ctx.db.invites, inv);
	send(inv);
	if (!try_invite.errors.empty()) { return false; }
	log(ctx, fmt("Saved new invite: %0", inv.to));
	db::commit(trans, fmt("Created invite: ", inv.to));
	return true;
      });

    rdr.cmds.emplace("lock", [&ctx](auto args) {
	if (!args.empty()) {
	  log(ctx, "Invalid number of arguments, syntax: lock");
	  return false;
	}
	gtk_widget_hide(left_panel);
	push_view(new Login(ctx));
	return true;
      });

    rdr.cmds.emplace("peer", [&ctx](auto args) {
	if (args.size() != 1) {
	  log(ctx, "Invalid number of arguments, syntax: peer ea362b58");
	  return false;
	}
	
	PeerSearch *v = new PeerSearch(ctx);
	auto id(args.back());
	gui::set_str(GTK_ENTRY(v->id_fld), id);

	if (find(*v) == 1) {
	  auto rec(first_rec(*v));
	  CHECK(rec != nullptr, _);
	  push_view(new PeerView(Peer(ctx, *rec)));
	  delete v;
	} else {
	  push_view(v);
	}
	
	return true;
      });
    
    rdr.cmds.emplace("peer-search", [&ctx](auto args) {
	if (!args.empty()) {
	  log(ctx, "Invalid number of arguments, syntax: peer-search");
	  return false;
	}
	
	push_view(new PeerSearch(ctx));
	return true;
      });

    rdr.cmds.emplace("post", [&ctx](auto args) {
	if (args.size() != 1) {
	  log(ctx, "Invalid number of arguments, syntax: post ea362b58");
	  return false;
	}
	
	PostSearch *v = new PostSearch(ctx);
	auto id(args.back());
	gui::set_str(GTK_ENTRY(v->id_fld), id);

	if (find(*v) == 1) {
	  auto rec(first_rec(*v));
	  CHECK(rec != nullptr, _);
	  push_view(new PostView(Post(ctx, *rec)));
	  delete v;
	} else {
	  push_view(v);
	}
	
	return true;
      });
    
    rdr.cmds.emplace("post-new", [&ctx](auto args) {
	if (!args.empty()) {
	  log(ctx, "Invalid number of arguments, syntax: post");
	  return false;
	}
	
	push_view(new PostView(Post(ctx)));
	return true;
      });

    rdr.cmds.emplace("post-search", [&ctx](auto args) {
	if (!args.empty()) {
	  log(ctx, "Invalid number of arguments, syntax: post-search");
	  return false;
	}
	
	push_view(new PostSearch(ctx));
	return true;
      });

    rdr.cmds.emplace("project", [&ctx](auto args) {
	if (args.size() != 1) {
	  log(ctx, "Invalid number of arguments, syntax: project ea362b58");
	  return false;
	}
	
	ProjectSearch *v = new ProjectSearch(ctx);
	auto id(args.back());
	gui::set_str(GTK_ENTRY(v->id_fld), id);

	if (find(*v) == 1) {
	  auto rec(first_rec(*v));
	  CHECK(rec != nullptr, _);
	  push_view(new ProjectView(Project(ctx, *rec)));
	  delete v;
	} else {
	  push_view(v);
	}
	
	return true;
      });
    
    rdr.cmds.emplace("project-new", [&ctx](auto args) {
	if (!args.empty()) {
	  log(ctx, "Invalid number of arguments, syntax: project");
	  return false;
	}
	
	push_view(new ProjectView(Project(ctx)));
	return true;
      });

    rdr.cmds.emplace("project-search", [&ctx](auto args) {
	if (!args.empty()) {
	  log(ctx, "Invalid number of arguments, syntax: project-search");
	  return false;
	}
	
	push_view(new ProjectSearch(ctx));
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

	if (!setup) { setup.reset(new Setup(ctx)); }
	push_view(setup.get());
	return true;
      });

    rdr.cmds.emplace("task", [&ctx](auto args) {
	if (args.size() != 1) {
	  log(ctx, "Invalid number of arguments, syntax: task ea362b58");
	  return false;
	}
	
	TaskSearch *v = new TaskSearch(ctx);
	auto id(args.back());
	gui::set_str(GTK_ENTRY(v->id_fld), id);

	if (find(*v) == 1) {
	  auto rec(first_rec(*v));
	  CHECK(rec != nullptr, _);
	  push_view(new TaskView(Task(ctx, *rec)));
	  delete v;
	} else {
	  push_view(v);
	}
	
	return true;
      });

    rdr.cmds.emplace("task-new", [&ctx](auto args) {
	if (!args.empty()) {
	  log(ctx, "Invalid number of arguments, syntax: task");
	  return false;
	}
	
	push_view(new TaskView(Task(ctx)));
	return true;
      });

    rdr.cmds.emplace("task-search", [&ctx](auto args) {
	if (!args.empty()) {
	  log(ctx, "Invalid number of arguments, syntax: task-search");
	  return false;
	}
	
	push_view(new TaskSearch(ctx));
	return true;
      });
    
    rdr.cmds.emplace("todo", [&ctx](auto args) {
	if (args.empty()) {
	  auto v(new TaskSearch(ctx));
	  set_str(GTK_ENTRY(v->tags_fld), "todo");
	  push_view(v);
	} else {
	  Task task(ctx);
	  task.tags.insert("todo");
	  task.name = join(args.begin(), args.end(), ' ');
	  push_view(new TaskView(task));
	}
	
	return true;
      });

    rdr.cmds.emplace("undo", [&ctx](auto args) {
	if (!args.empty()) {
	  log(ctx, "Invalid number of arguments, syntax: inbox");
	  return false;
	}

	if (ctx.undo_stack.empty()) {
	  log(ctx, "Nothing to undo");
	  return true;
	}

	if (!undo) { undo.reset(new Undo(ctx)); }
	push_view(undo.get());
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
  
  static opt<Reader::Cmd> find_cmd(Reader &rdr, const str &_in) {
    str in(_in);
    
    if (in.empty()) {
      if (!rdr.last_cmd) { return nullopt; }
      in = *rdr.last_cmd;
    }

    auto parsed(parse_cmd(rdr, in));

    auto found(rdr.cmds.find(parsed.first));
    if (found == rdr.cmds.end()) { return nullopt; }
    return found->second; 
  }

  static bool exec_cmd(Reader &rdr, const str &in) {
    Ctx &ctx(rdr.ctx);
    TRY(try_exec);

    auto parsed(parse_cmd(rdr, in));
    auto cmd(find_cmd(rdr, parsed.first));
      
    if (!cmd){
      log(ctx, fmt("Unknown command: '%0'", in));
      return false;
    }
      
    if (!(*cmd)(parsed.second)) { return false; }
      
    if (!in.empty()) {
      rdr.last_cmd = in;
      gtk_entry_set_placeholder_text(GTK_ENTRY(rdr.entry), in.c_str());
    }
    
    return try_exec.errors.empty();
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

    g_signal_connect(G_OBJECT(entry),
		     "key_press_event",
		     G_CALLBACK(on_key),
		     this);
  }
  
  GtkWidget *Reader::ptr() {
    return entry;
  }
}}
