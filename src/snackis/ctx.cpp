#include <chrono>
#include "snackis/ctx.hpp"
#include "snackis/db/error.hpp"
#include "snackis/net/imap.hpp"
#include "snackis/net/smtp.hpp"

namespace snackis {
  static void fetch_loop(Ctx *ctx) {
    while (!ctx->is_closing) {
      std::unique_lock<std::mutex> lock(ctx->loop_mutex);
      int64_t poll(0);
      
      {
	std::unique_lock<std::recursive_mutex> lock(ctx->mutex);
	poll = *get_val(ctx->settings.imap_poll);
      }
      
      if (poll) {
	ctx->fetch_cond.wait_for(lock, std::chrono::seconds(poll));
      } else {
	ctx->fetch_cond.wait(lock);
      }

      if (!ctx->is_closing) {
	try {
	  Imap imap(*ctx);
	  fetch(imap);
	} catch (const std::exception &e) {
	  log(*ctx, fmt("Failed fetching email: %0", e.what()));
	}
      }
    }
  }

  static void send_loop(Ctx *ctx) {
    while (!ctx->is_closing) {
      std::unique_lock<std::mutex> send_lock(ctx->loop_mutex);
      int64_t poll(0);
      
      {
	std::unique_lock<std::recursive_mutex> lock(ctx->mutex);
	poll = *get_val(ctx->settings.smtp_poll);
      }
      
      if (poll) {
	ctx->send_cond.wait_for(send_lock, std::chrono::seconds(poll));	
      } else {
	ctx->send_cond.wait(send_lock);
      }

      std::unique_lock<std::recursive_mutex> lock(ctx->mutex);
      if (!ctx->is_closing && !ctx->db.outbox.recs.empty()) {
	try {
	  Smtp smtp(*ctx);
	  send(smtp);
	} catch (const std::exception &e) {
	  log(*ctx, fmt("Failed sending email: %0", e.what()));
	}
      }
    }
  }

  Ctx::Ctx(const Path &path):
    db::Ctx(path), db(*this), settings(*this), whoami(*this), is_closing(false)
  { }

  Ctx::~Ctx() {
    is_closing = true;

    if (fetcher) {
      fetch_cond.notify_one();
      fetcher->join();
    }

    if (sender) {
      send_cond.notify_one();
      sender->join();
    }
  }
  
  void open(Ctx &ctx) {
    TRACE("Opening Snackis context");
    open(dynamic_cast<db::Ctx &>(ctx));
    create_path(*get_val(ctx.settings.load_folder));
    create_path(*get_val(ctx.settings.save_folder));
    slurp(ctx);
    db::Trans trans(ctx);
    opt<UId> me_id = get_val(ctx.settings.whoami);
    
    if (me_id) {
      ctx.whoami.id = *me_id;
      if (!load(ctx.db.peers, ctx.whoami)) {
	ERROR(Db, "Failed loading me");
      }
    } else {
      set_val(ctx.settings.whoami, ctx.whoami.id);
    }
    
    opt<crypt::Key> found_crypt_key(get_val(ctx.settings.crypt_key));
    if (!found_crypt_key) {
      crypt::Key key(ctx.whoami.crypt_key);
      set_val(ctx.settings.crypt_key, key);
      log(ctx, "Initialized encryption-key");
    }
    
    db::upsert(ctx.db.peers, ctx.whoami);
    db::commit(trans);
    ctx.fetcher.emplace(fetch_loop, &ctx);
    ctx.sender.emplace(send_loop, &ctx);
  }

  void log(const Ctx &ctx, const str &msg) { db::log(ctx,msg); }

  Peer &whoami(Ctx &ctx) {
    if (!load(ctx.db.peers, ctx.whoami)) {
      ERROR(Db, "Failed loading me");
    }

    return ctx.whoami;
  }

  Queue todo_queue(Ctx &ctx) {
    auto id(get_val(ctx.settings.todo_queue));

    if (!id) {
      db::Trans trans(ctx);
      Queue queue(ctx);
      queue.name = "Todo";
      set_val(ctx.settings.todo_queue, queue.id);
      db::insert(ctx.db.queues, queue);
      db::commit(trans);
      log(ctx, "Initialized Todo queue");
      return queue;
    }
    
    return get_queue_id(ctx, *id);
  }
}
