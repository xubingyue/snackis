#include "snackis/ctx.hpp"
#include "snackis/net/smtp_worker.hpp"
#include "snackis/net/smtp.hpp"

namespace snackis {
namespace net {
  SmtpWorker::SmtpWorker(Ctx &ctx): Worker(ctx) {
    db::copy(this->ctx.db.outbox, ctx.db.outbox);
    start(*this);
  }
  
  void SmtpWorker::run() {
    error_handler = [this](auto &errors) {
      for (auto e: errors) { log(ctx, e->what); }
    };

    while (running) {
      TRY(try_smtp);
      auto poll(*get_val(ctx.settings.smtp.poll));
      
      if (poll) {
	SmtpWorker::Lock lock(mutex);
   	go.wait_for(lock, std::chrono::seconds(poll));	
      } else {
	SmtpWorker::Lock lock(mutex);
   	go.wait(lock);
      }

      if (!running) { break; }

      refresh(ctx);
      if (!ctx.db.outbox.recs.empty()) {
	Smtp smtp(ctx);
	send(smtp);
      }
    }
  }
}}
