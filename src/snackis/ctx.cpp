#include <chrono>
#include "snackis/ctx.hpp"
#include "snackis/db/error.hpp"
#include "snackis/net/imap.hpp"
#include "snackis/net/smtp.hpp"

namespace snackis {
  static void fetch_loop(Ctx *ctx) {
    {
      std::unique_lock<std::mutex> lock(ctx->fetch_mutex);
      ctx->fetch_cond.wait(lock);
    }

    while (!ctx->is_closing) {
      auto poll(*get_val(ctx->settings.imap_poll));
      std::unique_lock<std::mutex> lock(ctx->fetch_mutex);
      
      if (poll) {
	ctx->fetch_cond.wait_for(lock, std::chrono::seconds(poll));

	if (!ctx->is_closing) {
	  Imap imap(*ctx);
	  fetch(imap);
	}
      } else {
	ctx->fetch_cond.wait(lock);
      }
    }
  }

  static void send_loop(Ctx *ctx) {
    {
      std::unique_lock<std::mutex> lock(ctx->send_mutex);
      ctx->send_cond.wait(lock);
    }

    while (!ctx->is_closing) {
      auto poll(*get_val(ctx->settings.smtp_poll));
      std::unique_lock<std::mutex> lock(ctx->send_mutex);
      
      if (poll) {
	ctx->send_cond.wait_for(lock, std::chrono::seconds(poll));
	
	if (!ctx->is_closing && !ctx->db.outbox.recs.empty()) {
	  Smtp smtp(*ctx);
	  send(smtp);
	}
      } else {
	ctx->send_cond.wait(lock);
      }
    }
  }

  Ctx::Ctx(const Path &path):
    db::Ctx(path), db(*this), settings(*this), whoami(*this),
    fetcher(fetch_loop, this),
    sender(send_loop, this),
    is_closing(false)
  { }

  Ctx::~Ctx() {
    is_closing = true;
    fetch_cond.notify_one();
    fetcher.join();
    send_cond.notify_one();
    sender.join();
  }
  
  void open(Ctx &ctx) {
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
      log(ctx, "New encryption-key created");
    }
    
    db::upsert(ctx.db.peers, ctx.whoami);
    db::commit(trans);
    ctx.fetch_cond.notify_one();
    ctx.send_cond.notify_one();
  }

  void log(const Ctx &ctx, const str &msg) { db::log(ctx,msg); }

  Peer &whoami(Ctx &ctx) {
    if (!load(ctx.db.peers, ctx.whoami)) {
      ERROR(Db, "Failed loading me");
    }

    return ctx.whoami;
  }
}
