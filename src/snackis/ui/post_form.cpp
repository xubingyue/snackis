#include "snackis/ctx.hpp"
#include "snackis/db/ctx.hpp"
#include "snackis/ui/post_form.hpp"
#include "snackis/ui/view.hpp"
#include "snackis/ui/window.hpp"

namespace snackis {
namespace ui {
  PostForm::PostForm(View &view, Footer &ftr):
    ViewForm(view, ftr),
    thread(*this, Dim(1, view.dim.w), "Existing Thread"),
    subject(*this, Dim(1, view.dim.w), "New Subject"),
    peer(*this, "Peer"),
    send_to(*this, Dim(5, view.dim.w), "Send To"),
    body(*this, Dim(5, view.dim.w), "Body") {
    label = "Post";
    status = "Press Ctrl-s to post, or Ctrl-q to cancel";
    margin_top = 1;

    for (auto p: ctx.db.threads.recs) {
      insert(thread, *get(p, ctx.db.thread_subject), *get(p, ctx.db.thread_id));
    }

    thread.allow_clear = true;
    thread.on_change = [this]() {
      if (thread.selected) { set_str(subject, ""); }
    };
    
    subject.on_change = [this]() {
      if (!get_str(subject).empty()) { clear(thread, false); }
    };

    peer.name.margin_top = 1;
    send_to.active = false;
    send_to.info = "Press Return to add/remove specified peer";

    body.margin_top = 2;
  }

  bool run(PostForm &frm) {
    Ctx &ctx(frm.window.ctx);
    db::Trans trans(ctx);
    
    while (true) {
      chtype ch = get_key(frm.window);
      
      if (ch == KEY_CTRL('s')) {
	validate(frm);
	db::Rec<Thread> thread_rec;
	
	if (frm.thread.selected) {
	  set(thread_rec, ctx.db.thread_id, frm.thread.selected->val);
	  load(ctx.db.threads, thread_rec);
	} else {
	  Thread thread(ctx);
	  thread.subject = get_str(frm.subject);
	  copy(ctx.db.peers.key, thread.started_by, whoami(ctx));
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

      switch (ch) {
      case KEY_CTRL('q'):
	return false;
      default:
	drive(frm, ch);
      }
    }
  }
}}
