#include "snackis/thread.hpp"

namespace snackis {
  Thread::Thread(Ctx &ctx): Rec(ctx), id(uid()), started_at(now()) { }
}
