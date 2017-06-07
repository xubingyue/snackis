#include "snackis/ctx.hpp"
#include "snackis/db/error.hpp"

namespace snackis {
  Ctx::Ctx(const Path &path):
    db::Ctx(path), db(*this), settings(*this), whoami(*this) 
  { }

  void open(Ctx &ctx) {
    open(dynamic_cast<db::Ctx &>(ctx));
    create_path(*get_val(ctx.settings.load_folder));
    create_path(*get_val(ctx.settings.save_folder));

    slurp(ctx);

    db::Trans trans(ctx);

    opt<db::Rec<Peer>> found_me = get_val(ctx.settings.whoami);
    if (found_me) {
      copy(ctx.db.peers, ctx.whoami, *found_me);
      if (!load(ctx.db.peers, ctx.whoami)) {
	ERROR(Db, "Failed loading me");
      }
    } else {
      db::Rec<Peer> me;
      copy(ctx.db.peers.key, me, ctx.whoami);
      set_val(ctx.settings.whoami, me);
    }
    
    opt<crypt::Key> found_crypt_key(get_val(ctx.settings.crypt_key));
    if (!found_crypt_key) {
      crypt::Key key(ctx.whoami.crypt_key);
      set_val(ctx.settings.crypt_key, key);
      log(ctx, "New encryption-key created");
    }
    
    db::upsert(ctx.db.peers, ctx.whoami);
    db::commit(trans);
  }

  void log(const Ctx &ctx, const str &msg) { db::log(ctx,msg); }

  Peer &whoami(Ctx &ctx) {
    if (!load(ctx.db.peers, ctx.whoami)) {
      ERROR(Db, "Failed loading me");
    }

    return ctx.whoami;
  }
}
