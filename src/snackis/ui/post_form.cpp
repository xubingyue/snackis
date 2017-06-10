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

  PostForm::PostForm(View &view, Footer &ftr):
    ViewForm(view, ftr),
    peers([this](const auto &x, const auto &y) {
	return *get(x, ctx.db.peer_name) < *get(y, ctx.db.peer_name);
      }),
    thread(*this, Dim(1, view.dim.w), "Existing Thread"),
    subject(*this, Dim(1, view.dim.w), "New Subject"),
    peer(*this, "Peer"),
    send_to(*this, Dim(5, view.dim.w), "Send To"),
    body(*this, Dim(5, view.dim.w), "Body") {
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
	for (const auto &p: thread.peers) { peers.insert(p); }
	update_peers(*this);
      }
    };
    
    subject.on_change = [this]() {
      if (!get_str(subject).empty()) { clear(thread, false); }
    };

    peer.name.margin_top = 1;
    send_to.active = false;
    send_to.info = "Press Return to add/remove specified peer";

    body.margin_top = 2;
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
  
  bool run(PostForm &frm) {
    Ctx &ctx(frm.window.ctx);
    db::Trans trans(ctx);
    
    while (true) {
      chtype ch = get_key(frm.window);

      switch (ch) {
      case KEY_RETURN: {
	Field &f(active_field(frm));
	
	if (frm.peer.selected &&
	    (f.ptr == frm.peer.name.ptr || f.ptr == frm.peer.email.ptr)) {
	  db::Rec<Peer> rec;
	  set(rec, frm.ctx.db.peer_id, *frm.peer.selected);
	  load(frm.ctx.db.peers, rec);
	  toggle_peer(frm, rec);
	  clear(dynamic_cast<EnumField<UId> &>(f));
	} else {
	  drive(frm, ch);
	}
	
	break;
      }
      case KEY_CTRL('s'): {
	validate(frm);
	db::Rec<Thread> thread_rec;
	
	if (frm.thread.selected) {
	  set(thread_rec, ctx.db.thread_id, frm.thread.selected->val);
	  load(ctx.db.threads, thread_rec);
	  Thread thread(ctx.db.threads, thread_rec);
	  thread.peers.assign(frm.peers.begin(), frm.peers.end());
	  copy(ctx.db.threads, thread_rec, thread);
	  update(ctx.db.threads, thread_rec);
	} else {
	  Thread thread(ctx);
	  thread.subject = get_str(frm.subject);
	  copy(ctx.db.peers.key, thread.started_by, whoami(ctx));
	  thread.peers.assign(frm.peers.begin(), frm.peers.end());
	  copy(ctx.db.threads, thread_rec, thread);
	  insert(ctx.db.threads, thread_rec);
	  log(ctx, fmt("New thread created: %0", thread.subject));
	}

	
	Post post(ctx, thread_rec);
	post.at = now();
	copy(ctx.db.peers.key, post.by, whoami(ctx));
	post.body = get_str(frm.body);
	insert(ctx.db.posts, post);
	
	log(ctx, "New post created");
	db::commit(trans);
	return true;
      }
      case KEY_CTRL('q'):
	return false;
      default:
	drive(frm, ch);
      }
    }
  }
}}
