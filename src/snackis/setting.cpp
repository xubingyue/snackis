#include "snackis/ctx.hpp"
#include "snackis/setting.hpp"
#include "snackis/db/table.hpp"

namespace snackis {
  db::Col<BasicSetting, str> setting_key("key", str_type, &BasicSetting::key);
  db::Col<BasicSetting, str> setting_val("val", str_type, &BasicSetting::val);
  db::Key<BasicSetting, str> settings_key(setting_key);

  BasicSetting::BasicSetting(Ctx &ctx, const str &key): Rec(ctx), key(key)
  { }

  BasicSetting::BasicSetting(Ctx &ctx, const db::Rec<BasicSetting> &rec): Rec(ctx) {
    copy(*this, rec);
  }
}
