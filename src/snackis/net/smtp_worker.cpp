#include "snackis/ctx.hpp"
#include "snackis/net/smtp_worker.hpp"
#include "snackis/net/smtp.hpp"

namespace snackis {
namespace net {
  static void loop(SmtpWorker *w) {
    Ctx &ctx(w->ctx);
    
    error_handler = [&ctx](auto &errors) {
      for (auto e: errors) { log(ctx, e->what); }
    };

    while (!w->stopped) {
      TRY(try_smtp);
      auto poll(*get_val(ctx.settings.smtp.poll));
      
      if (poll) {
	SmtpWorker::Lock lock(w->mutex);
   	w->go.wait_for(lock, std::chrono::seconds(poll));	
      } else {
	SmtpWorker::Lock lock(w->mutex);
   	w->go.wait(lock);
      }

      if (w->stopped) { break; }

      refresh(ctx);
      if (!ctx.db.outbox.recs.empty()) {
	Smtp smtp(ctx);
	send(smtp);
      }
    }
  }

  SmtpWorker::SmtpWorker(Ctx &ctx):
    ctx(ctx.proc, ctx.inbox.max),
    stopped(false) {
    this->ctx.secret = ctx.secret;
    db::copy(this->ctx.db.settings, ctx.db.settings);
    db::copy(this->ctx.db.outbox, ctx.db.outbox);
    thread = std::thread(loop, this);
  }
  
  SmtpWorker::~SmtpWorker() {
    stopped = true;
    go.notify_one();
    thread.join();
  }
}}
