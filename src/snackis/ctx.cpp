#include "snackis/ctx.hpp"

namespace snackis {
  Ctx::Ctx(db::Ctx &db_ctx): db(db_ctx), settings(*this) { }
}
