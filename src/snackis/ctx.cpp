#include <chrono>
#include "snackis/ctx.hpp"
#include "snackis/db/error.hpp"
#include "snackis/net/imap.hpp"

namespace snackis {
  static void fetch_loop(Ctx *ctx) {
    {
      std::unique_lock<std::mutex> lock(ctx->fetch_mutex);
      ctx->fetch_cond.wait(lock);
    }
    
    while (!ctx->is_closing) {
      auto freq(get_val(ctx->settings.imap_freq));
      std::unique_lock<std::mutex> lock(ctx->fetch_mutex);
      
      if (freq) {
	ctx->fetch_cond.wait_for(lock, std::chrono::seconds(*freq));
	if (!ctx->is_closing) {
	  Imap imap(*ctx);
	  fetch(imap);
	}
      } else {
	ctx->fetch_cond.wait(lock);
      }
    }
  }

  Ctx::Ctx(const Path &path):
    db::Ctx(path), db(*this), settings(*this), whoami(*this),
    fetcher(fetch_loop, this),
    is_closing(false)
  { }

  Ctx::~Ctx() {
    is_closing = true;
    fetch_cond.notify_one();
    fetcher.join();
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
  }

  void log(const Ctx &ctx, const str &msg) { db::log(ctx,msg); }

  Peer &whoami(Ctx &ctx) {
    if (!load(ctx.db.peers, ctx.whoami)) {
      ERROR(Db, "Failed loading me");
    }

    return ctx.whoami;
  }
}
