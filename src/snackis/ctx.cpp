#include "snackis/ctx.hpp"

namespace snackis {
  Ctx::Ctx(db::Ctx &db_ctx): db(db_ctx), settings(*this) { }

  void log(Ctx &ctx, const str &msg) {
    if (ctx.log) {
      WLock lock(ctx.log_mutex);
      ctx.log.get()(msg);
    }
  }
}
