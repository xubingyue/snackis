#include <chrono>
#include "snackis/ctx.hpp"
#include "snackis/db/error.hpp"
#include "snackis/net/imap.hpp"
#include "snackis/net/smtp.hpp"

namespace snackis {
  static void fetch_loop(Ctx *ctx) {
    error_handler = [ctx](auto &errors) {
      for (auto e: errors) { log(*ctx, e->what); }
    };

    while (!ctx->closing) {
      TRY(try_imap);
      Ctx::LoopLock lock(ctx->loop_mutex);
      int64_t poll(0);
      
      {
	Ctx::Lock lock(ctx->mutex);
	poll = *get_val(ctx->settings.imap.poll);
      }
      
      if (poll) {
	ctx->fetch_cond.wait_for(lock, std::chrono::seconds(poll));
      } else {
	ctx->fetch_cond.wait(lock);
      }

      if (!ctx->closing) {
	Imap imap(*ctx);
	fetch(imap);
      }
    }
  }

  static void send_loop(Ctx *ctx) {
    error_handler = [ctx](auto &errors) {
      for (auto e: errors) { log(*ctx, e->what); }
    };

    while (!ctx->closing) {
      TRY(try_smtp);
      Ctx::LoopLock send_lock(ctx->loop_mutex);
      int64_t poll(0);
      
      {
	Ctx::Lock lock(ctx->mutex);
	poll = *get_val(ctx->settings.smtp.poll);
      }
      
      if (poll) {
	ctx->send_cond.wait_for(send_lock, std::chrono::seconds(poll));	
      } else {
	ctx->send_cond.wait(send_lock);
      }

      Ctx::Lock lock(ctx->mutex);
      if (!ctx->closing && !ctx->db.outbox.recs.empty()) {
	Smtp smtp(*ctx);
	send(smtp);
      }
    }
  }

  Ctx::Ctx(db::Proc &p, size_t max_buf):
    db::Ctx(p, max_buf), db(*this), settings(*this), whoami(*this), closing(false)
  { }

  Ctx::~Ctx() {
    closing = true;

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
    db::Trans trans(ctx);
    TRY(try_open);
    
    open(dynamic_cast<db::Ctx &>(ctx));
    create_path(*get_val(ctx.settings.load_folder));
    create_path(*get_val(ctx.settings.save_folder));
    slurp(ctx);
    opt<UId> me_id = get_val(ctx.settings.whoami);
    
    if (me_id) {
      ctx.whoami.id = *me_id;
      if (!load(ctx.db.peers, ctx.whoami)) {
	ERROR(Db, "Failed loading me");
	return;
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
    if (try_open.errors.empty()) { db::commit(trans, nullopt); }
    ctx.fetcher.emplace(fetch_loop, &ctx);
    ctx.sender.emplace(send_loop, &ctx);
  }

  void log(const Ctx &ctx, const str &msg) { db::log(ctx,msg); }

  Peer &whoami(Ctx &ctx) {
    CHECK(load(ctx.db.peers, ctx.whoami), _);
    return ctx.whoami;
  }
  
  int64_t rewrite_db(Ctx &ctx) {
    db::Trans trans(ctx);
    rewrite_table(ctx.db.feeds);
    rewrite_table(ctx.db.posts);
    rewrite_table(ctx.db.projects);
    rewrite_table(ctx.db.tasks);
    auto out(db::rewrite(ctx));
    db::commit(trans, nullopt);
    return out;
  }
}
