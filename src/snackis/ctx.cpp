#include <chrono>
#include "snackis/ctx.hpp"
#include "snackis/db/error.hpp"
#include "snackis/net/imap.hpp"
#include "snackis/net/smtp.hpp"

namespace snackis {
  Ctx::Ctx(db::Proc &p, size_t max_buf):
    db::Ctx(p, max_buf), db(*this), settings(*this)
  { }

  void open(Ctx &ctx) {
    TRACE("Opening Snackis context");
    db::Trans trans(ctx);
    TRY(try_open);
    
    create_path(*get_val(ctx.settings.load_folder));
    create_path(*get_val(ctx.settings.save_folder));
    slurp(ctx);

    opt<UId> me_id = get_val(ctx.settings.whoami);
    if (!me_id) {
      Peer me(ctx);
      crypt::Key key(me.crypt_key);
      set_val(ctx.settings.crypt_key, key);
      set_val(ctx.settings.whoami, me.id);
      db::insert(ctx.db.peers, me);
      log(ctx, "Initialized encryption-key");
    }
    
    if (try_open.errors.empty()) { db::commit(trans, nullopt); }
  }

  void log(const Ctx &ctx, const str &msg) { db::log(ctx,msg); }

  UId whoamid(Ctx &ctx) {
    return *get_val(ctx.settings.whoami);
  }

  Peer whoami(Ctx &ctx) {
    return get_peer_id(ctx, whoamid(ctx));
  }

  int64_t rewrite_db(Ctx &ctx) {
    db::Trans trans(ctx);
    auto out(db::rewrite(ctx));
    db::commit(trans, nullopt);
    return out;
  }
}
