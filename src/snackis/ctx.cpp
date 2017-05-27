#include "snackis/ctx.hpp"
#include "snackis/db/error.hpp"

namespace snackis {
  Ctx::Ctx(const Path &path):
    db::Ctx(path), db(*this), settings(*this), whoami(*this) 
  { }

  void init_defaults(Ctx &ctx) {
    db::Trans trans(ctx);

    opt<Peer *> found_me = get_val(ctx.settings.whoami);
    if (found_me) {
      copy(ctx.db.peers, ctx.whoami, **found_me);
    } else {
      set_val(ctx.settings.whoami, &ctx.whoami);
    }
    
    opt<crypt::Key> found_crypt_key(get_val(ctx.settings.crypt_key));
    if (!found_crypt_key) {
      crypt::Key key(ctx.whoami.crypt_key);
      set_val(ctx.settings.crypt_key, key);
      log(ctx, "New crypto-key generated");
    }
    
    db::upsert(ctx.db.peers, ctx.whoami);
    db::commit(trans);
  }

  void log(const Ctx &ctx, const str &msg) { db::log(ctx,msg); }

  Peer &whoami(Ctx &ctx) {
    if (!load(ctx.db.peers, ctx.whoami)) {
      ERROR(db::Db, "Failed loading whoami");
    }

    return ctx.whoami;
  }
}
