#include "snackis/ctx.hpp"
#include "snackis/thread.hpp"

namespace snackis {
  Thread::Thread(Ctx &ctx): Rec(ctx), id(uid()), started_at(now()) { }

  Thread::Thread(const db::Table<Thread> &tbl, const db::Rec<Thread> &rec):
    Rec(dynamic_cast<Ctx &>(ctx)) {
    copy(tbl, *this, rec);
  }
}
