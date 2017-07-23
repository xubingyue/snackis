#include "snackis/ctx.hpp"
#include "snackis/net/imap_worker.hpp"
#include "snackis/net/imap.hpp"

namespace snackis {
namespace net {
  ImapWorker::ImapWorker(Ctx &ctx): Worker(ctx) {
    db::copy(this->ctx.db.invites, ctx.db.invites);
    db::copy(this->ctx.db.feeds, ctx.db.feeds);
    db::copy(this->ctx.db.posts, ctx.db.posts);
    db::copy(this->ctx.db.projects, ctx.db.projects);
    db::copy(this->ctx.db.tasks, ctx.db.tasks);
    start(*this);
  }
  
  void ImapWorker::run() {
    error_handler = [this](auto &errors) {
      for (auto e: errors) { log(ctx, e->what); }
    };

    while (running) {
      TRY(try_imap);
      auto poll(*get_val(ctx.settings.imap.poll));
      
      if (poll) {
	ImapWorker::Lock lock(mutex);
	go.wait_for(lock, std::chrono::seconds(poll));
      } else {
	ImapWorker::Lock lock(mutex);
	go.wait(lock);
      }

      if (!running) { break; }

      refresh(ctx);
      Imap imap(ctx);
      fetch(imap);
    }
  }
}}
