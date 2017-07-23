#include "snackis/ctx.hpp"
#include "snackis/net/imap_worker.hpp"
#include "snackis/net/imap.hpp"

namespace snackis {
namespace net {
  static void loop(ImapWorker *w) {
    Ctx &ctx(w->ctx);

    error_handler = [&ctx](auto &errors) {
      for (auto e: errors) { log(ctx, e->what); }
    };

    while (!w->stopped) {
      TRY(try_imap);
      auto poll(*get_val(ctx.settings.imap.poll));
      
      if (poll) {
	ImapWorker::Lock lock(w->mutex);
	w->go.wait_for(lock, std::chrono::seconds(poll));
      } else {
	ImapWorker::Lock lock(w->mutex);
	w->go.wait(lock);
      }

      if (w->stopped) { break; }

      refresh(ctx);
      Imap imap(ctx);
      fetch(imap);
    }
  }

  ImapWorker::ImapWorker(Ctx &ctx):
    ctx(ctx.proc, ctx.inbox.max),
    stopped(false) {
    this->ctx.secret = ctx.secret;
    db::copy(this->ctx.db.settings, ctx.db.settings);
    db::copy(this->ctx.db.peers, ctx.db.peers);
    db::copy(this->ctx.db.invites, ctx.db.invites);
    db::copy(this->ctx.db.feeds, ctx.db.feeds);
    db::copy(this->ctx.db.posts, ctx.db.posts);
    db::copy(this->ctx.db.projects, ctx.db.projects);
    db::copy(this->ctx.db.tasks, ctx.db.tasks);
    thread = std::thread(loop, this);
  }
  
  ImapWorker::~ImapWorker() {
    stopped = true;
    go.notify_one();
    thread.join();
  }
}}
