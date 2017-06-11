#include "snackis/ctx.hpp"
#include "snackis/thread.hpp"

namespace snackis {
  Thread::Thread(Ctx &ctx): Rec(ctx), started_at(now()) { }

  Thread::Thread(const db::Table<Thread> &tbl, const db::Rec<Thread> &rec):
    Rec(dynamic_cast<Ctx &>(tbl.ctx)), id(false) {
    copy(tbl, *this, rec);
  }
}
