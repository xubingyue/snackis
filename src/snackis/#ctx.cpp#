#include "snackis/ctx.hpp"

namespace snackis {
  Ctx::Ctx(const Path &path):
    db::Ctx(path), db(*this), settings(*this)
  { }

  void log(const Ctx &ctx, const str &msg) { db::log(ctx,msg); }
}
