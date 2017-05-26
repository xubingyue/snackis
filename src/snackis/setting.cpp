#include "snackis/ctx.hpp"
#include "snackis/setting.hpp"
#include "snackis/core/buf.hpp"
#include "snackis/db/table.hpp"

namespace snackis {
  BasicSetting::BasicSetting(Ctx &ctx, const str &key): Rec(ctx), key(key)
  { }

  BasicSetting::BasicSetting(const db::Table<BasicSetting> &tbl,
			     const db::Rec<BasicSetting> &rec):
    Rec(dynamic_cast<Ctx &>(tbl.ctx)) {
    copy(tbl, *this, rec);
  }
}
