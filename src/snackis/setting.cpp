#include "snackis/ctx.hpp"
#include "snackis/setting.hpp"
#include "snackis/db/table.hpp"

namespace snackis {
  BasicSetting::BasicSetting(Ctx &ctx, const str &key): Rec(ctx), key(key)
  { }

  BasicSetting::BasicSetting(Ctx &ctx, const db::Rec<BasicSetting> &rec): Rec(ctx) {
    copy(*this, rec);
  }
}
