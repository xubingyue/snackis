#include "snackis/ctx.hpp"
#include "snackis/db/ctx.hpp"
#include "snackis/ui/post_form.hpp"
#include "snackis/ui/view.hpp"
#include "snackis/ui/window.hpp"

namespace snackis {
namespace ui {
  static void update_peers(PostForm &frm) {
    Stream buf;
    str sep;
    
    for (const auto &p: frm.peers) {
      buf << fmt("%0 %1 (%2)", sep, p.name, p.email);
      sep = ", ";
    }

    set_str(frm.send_to, buf.str());
  }
  
  static str load_history(const Thread &thread, size_t width) {
    Ctx &ctx(thread.ctx);
	       
    Stream out;
    for (auto &post: last_posts(thread, 7)) {
      load(ctx.db.peers, post.by);
      Peer peer(ctx.db.peers, post.by);
      out << fill(fmt("%0 | %1 (%2)",
		      fmt(post.at, "%a %b %d, %H:%M:%S"),
		      peer.name, peer.email),
		  ' ', width);
      out << fill(post.body, ' ', width);
      out << fill("", ' ', width);
    }
    return out.str();
  }

  PostForm::PostForm(View &view, Footer &ftr):
    ViewForm(view, ftr),
    peers([this](const auto &x, const auto &y) { return x.name < y.name; }),
    thread(*this, Dim(1, view.dim.w), "Existing Thread"),
    subj(*this, Dim(1, view.dim.w), "New Subject"),
    body(*this, Dim(5, view.dim.w), "Body"),
    peer(*this, "Peer"),
    send_to(*this, Dim(5, view.dim.w), "Send To"),
    history(*this, Dim(view.dim.h-26, view.dim.w), "History") {
    label = "Post";
    status = "Press Ctrl-s to post, or Ctrl-q to cancel";
    margin_top = 1;

    for (auto t: ctx.db.threads.recs) {
      insert(thread, *get(t, ctx.db.thread_subj), *get(t, ctx.db.thread_id));
    }

    thread.allow_clear = true;
    thread.on_change = [this]() {
      if (thread.selected) {
	set_str(subj, "");
	set_str(history,
		load_history(get_thread_id(ctx, thread.selected->val),
			     history.dim.w));
      }
    };
    
    subj.on_change = [this]() {
      if (!get_str(subj).empty()) { clear(thread, false); }
    };

    peer.name.margin_top = 1;
    send_to.active = false;
    send_to.info = "Press Return to add/remove specified peer";

    body.margin_top = 1;
    body.rows = 10;
    history.margin_top = 2;
    history.editable = false;
    history.rows = 100;
  }
  
  static void toggle_peer(PostForm &frm, const Peer &peer) {
    auto found(frm.peers.find(peer));

    if (found == frm.peers.end()) {
      frm.peers.insert(peer);
    } else {
      frm.peers.erase(found);
    }

    update_peers(frm);
  }

  static void post(PostForm &frm, Thread &thread, const str &body) {
    Ctx &ctx(frm.ctx);
    Post post(thread);
    post.at = now();
    copy(ctx.db.peers.key, post.by, whoami(ctx));
    post.body = body;
    for (const auto &p: frm.peers) { post.peer_ids.insert(p.id); }
    insert(ctx.db.posts, post);
    log(ctx, "Saving messages to outbox...");
    post_msgs(post);
  }
  
  static bool save(PostForm &frm) {
    validate(frm);
    const str &subj(get_str(frm.subj));
    const str &body(get_str(frm.body));
    
    if ((!frm.thread.selected && subj == "") ||
	frm.peers.empty() ||
	body == "") {
      return false;
    }
    
    Ctx &ctx(frm.ctx);
    db::Trans trans(ctx);
    
    if (frm.thread.selected) {
      Thread thread(get_thread_id(ctx, frm.thread.selected->val));
      post(frm, thread, body);
    } else {
      Thread thread(ctx);
      thread.subj = subj;
      thread.by_id = whoami(ctx).id;
      insert(ctx.db.threads, thread);
      log(ctx, fmt("New thread created: %0", thread.subj));
      post(frm, thread, body);
    }
    
    db::commit(trans);
    log(ctx, "New post created");
    return true;
  }
  
  bool run(PostForm &frm) {
    Ctx &ctx(frm.ctx);
    
    while (true) {
      chtype ch = get_key(frm.window);

      switch (ch) {
      case KEY_RETURN: {
	Field &f(active_field(frm));
	
	if (frm.peer.selected &&
	    (f.ptr == frm.peer.name.ptr || f.ptr == frm.peer.email.ptr)) {
	  toggle_peer(frm, get_peer_id(ctx, *frm.peer.selected));
	  clear(dynamic_cast<EnumField<UId> &>(f));
	} else {
	  drive(frm, ch);
	}
	
	break;
      }
      case KEY_CTRL('s'):
	if (save(frm)) { return true; }
	break;
      case KEY_CTRL('q'):
	return false;
      default:
	drive(frm, ch);
      }
    }
  }
}}
