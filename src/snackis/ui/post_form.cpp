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
      buf <<
	sep << *get(p, frm.ctx.db.peer_name) <<
	'(' << *get(p, frm.ctx.db.peer_email) << ')';
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
    peers([this](const auto &x, const auto &y) {
	return *get(x, ctx.db.peer_name) < *get(y, ctx.db.peer_name);
      }),
    thread(*this, Dim(1, view.dim.w), "Existing Thread"),
    subject(*this, Dim(1, view.dim.w), "New Subject"),
    body(*this, Dim(5, view.dim.w), "Body"),
    peer(*this, "Peer"),
    send_to(*this, Dim(5, view.dim.w), "Send To"),
    history(*this, Dim(view.dim.h-26, view.dim.w), "History") {
    label = "Post";
    status = "Press Ctrl-s to post, or Ctrl-q to cancel";
    margin_top = 1;

    for (auto t: ctx.db.threads.recs) {
      insert(thread, *get(t, ctx.db.thread_subject), *get(t, ctx.db.thread_id));
    }

    thread.allow_clear = true;
    thread.on_change = [this]() {
      if (thread.selected) {
	set_str(subject, "");

	peers.clear();
	db::Rec<Thread> rec;
	set(rec, ctx.db.thread_id, thread.selected->val);
	load(ctx.db.threads, rec);
	Thread thread(ctx.db.threads, rec);

	for (const auto &id: thread.peer_ids) {
	  db::Rec<Peer> peer_rec;
	  set(peer_rec, ctx.db.peer_id, id);
	  load(ctx.db.peers, peer_rec);
	  peers.insert(peer_rec);
	}
	
	update_peers(*this);
	set_str(history, load_history(thread, history.dim.w));
      }
    };
    
    subject.on_change = [this]() {
      if (!get_str(subject).empty()) { clear(thread, false); }
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
  
  static void toggle_peer(PostForm &frm, const db::Rec<Peer> &peer) {
    auto found(frm.peers.find(peer));

    if (found == frm.peers.end()) {
      frm.peers.insert(peer);
    } else {
      frm.peers.erase(found);
    }

    update_peers(frm);
  }

  static bool save(PostForm &frm) {
    validate(frm);
    const str &subject(get_str(frm.subject));
    const str &body(get_str(frm.body));
    
    if ((!frm.thread.selected && subject == "") ||
	frm.peers.empty() ||
	body == "") { return false; }
    
    Ctx &ctx(frm.ctx);
    db::Trans trans(ctx);
    db::Rec<Thread> thread_rec;
    
    if (frm.thread.selected) {
      set(thread_rec, ctx.db.thread_id, frm.thread.selected->val);
      load(ctx.db.threads, thread_rec);
      Thread thread(ctx.db.threads, thread_rec);
      
      thread.peer_ids.clear();
      for (const auto &p: frm.peers) {
	thread.peer_ids.push_back(*get(p, ctx.db.peer_id));
      }
      
      copy(ctx.db.threads, thread_rec, thread);
      update(ctx.db.threads, thread_rec);
    } else {
      Thread thread(ctx);
      thread.subject = subject;
      copy(ctx.db.peers.key, thread.started_by, whoami(ctx));
      
      thread.peer_ids.clear();
      for (const auto &p: frm.peers) {
	thread.peer_ids.push_back(*get(p, ctx.db.peer_id));
      }
      
      copy(ctx.db.threads, thread_rec, thread);
      insert(ctx.db.threads, thread_rec);
      log(ctx, fmt("New thread created: %0", thread.subject));
    }

    Post post(ctx, thread_rec);
    post.at = now();
    copy(ctx.db.peers.key, post.by, whoami(ctx));
    post.body = body;
    insert(ctx.db.posts, post);
    log(ctx, "Saving messages to outbox...");
    post_msgs(post);
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
	  db::Rec<Peer> rec;
	  set(rec, ctx.db.peer_id, *frm.peer.selected);
	  load(ctx.db.peers, rec);
	  toggle_peer(frm, rec);
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
