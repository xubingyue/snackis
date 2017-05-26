#include "snackis/ctx.hpp"

namespace snackis {
  Ctx::Ctx(const Path &path):
    db::Ctx(path), db(*this), settings(*this)
  { }

  void log(const Ctx &ctx, const str &msg) {
    if (ctx.log) {
      WLock lock(const_cast<Ctx &>(ctx).log_mutex);
      ctx.log.get()(msg);
    }
  }
}
