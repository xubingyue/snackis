#include "snackis/ctx.hpp"
#include "snackis/db/error.hpp"

namespace snackis {
  Ctx::Ctx(const Path &path):
    db::Ctx(path), db(*this), settings(*this), whoami(*this) 
  { }

  void log(const Ctx &ctx, const str &msg) { db::log(ctx,msg); }

  Peer &whoami(Ctx &ctx) {
    if (!load(ctx.db.peers, ctx.whoami)) {
      ERROR(db::Db, "Failed loading whoami");
    }

    return ctx.whoami;
  }

  void open(Ctx &ctx) {
  }
}
