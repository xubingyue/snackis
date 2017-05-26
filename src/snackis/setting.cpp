#include "snackis/ctx.hpp"
#include "snackis/setting.hpp"
#include "snackis/core/buf.hpp"
#include "snackis/db/table.hpp"

namespace snackis {
  BasicSetting::BasicSetting(Ctx &ctx, const str &key, const BasicType &type): 
    Rec(ctx), key(key), type(type) { }

  BasicSetting::BasicSetting(const db::Table<BasicSetting> &tbl,
			     const db::Rec<BasicSetting> &rec):
    Rec(dynamic_cast<Ctx &>(tbl.ctx)), type(null_type) {
    copy(tbl, *this, rec);
  }
}
